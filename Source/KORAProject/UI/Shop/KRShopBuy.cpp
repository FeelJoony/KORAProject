// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Shop/KRShopBuy.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRItemUIData.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "UI/Modal/KRConfirmModal.h"

#include "CommonTextBlock.h"
#include "CommonButtonBase.h"
#include "CommonNumericTextBlock.h"
#include "Engine/Texture2D.h"
#include "Player/KRPlayerState.h"

class AKRPlayerState;
DEFINE_LOG_CATEGORY_STATIC(LogShopBuy, Log, All);

void UKRShopBuy::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("ShopBuy"));

		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &UKRShopBuy::HandleMoveLeft));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &UKRShopBuy::HandleMoveRight));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &UKRShopBuy::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &UKRShopBuy::HandleMoveDown));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &UKRShopBuy::HandleSelect));
	}
}

void UKRShopBuy::NativeOnDeactivated()
{

	Super::NativeOnDeactivated();
}

void UKRShopBuy::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshShopInventory();
	UpdatePlayerCurrency();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

		CurrencyListener = Subsys.RegisterListener(
			FKRUIMessageTags::Currency(),
			this,
			&ThisClass::OnCurrencyMessageReceived
		);
	}
}

void UKRShopBuy::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		Subsys.UnregisterListener(CurrencyListener);
	}

	Super::NativeDestruct();

}


void UKRShopBuy::OnShopItemSelected(int32 SelectedIndex)
{
	UpdateItemDescription(SelectedIndex);
}

void UKRShopBuy::UpdateItemDescription(int32 CellIndex)
{
	if (!ShopItemDescription) return;

	if (CachedShopItems.IsValidIndex(CellIndex))
	{
		ShopItemDescription->SetVisibility(ESlateVisibility::HitTestInvisible);
		const FKRItemUIData& D = CachedShopItems[CellIndex];
		ShopItemDescription->UpdateItemInfo(D.ItemNameKey, D.ItemDescriptionKey, D.ItemIcon);
	}
	else
	{
		ShopItemDescription->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKRShopBuy::HandleSelect()
{
	UE_LOG(LogShopBuy, Log, TEXT("[ShopBuy] HandleSelect called"));

	if (!ShoppingSlot)
	{
		UE_LOG(LogShopBuy, Warning, TEXT("[ShopBuy] ShoppingSlot is nullptr"));
		return;
	}

	const int32 Index = ShoppingSlot->GetSelectedIndex();
	UE_LOG(LogShopBuy, Log, TEXT("[ShopBuy] SelectedIndex = %d, CachedShopItems.Num = %d"), Index, CachedShopItems.Num());

	if (!CachedShopItems.IsValidIndex(Index))
	{
		UE_LOG(LogShopBuy, Warning, TEXT("[ShopBuy] Invalid index for CachedShopItems"));
		return;
	}

	const FKRItemUIData& ItemData = CachedShopItems[Index];
	FGameplayTag ItemTag = ItemData.ItemTag;

	UE_LOG(LogShopBuy, Log, TEXT("[ShopBuy] Selected item: %s"), *ItemTag.ToString());

	const FText Msg = FText::FromStringTable(
		TEXT("/Game/UI/StringTable/ST_UIBaseTexts"),
		TEXT("Modal_BuyConfirm")
	);

	if (UGameInstance* GI = GetGameInstance())
	{
		UE_LOG(LogShopBuy, Log, TEXT("[ShopBuy] GameInstance valid"));

		if (auto* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			UE_LOG(LogShopBuy, Log, TEXT("[ShopBuy] Router Subsystem valid, trying ToggleRoute(Confirm)"));

			if (auto* Widget = Router->ToggleRoute(TEXT("Confirm")))
			{
				UE_LOG(LogShopBuy, Log, TEXT("[ShopBuy] ToggleRoute returned widget: %s"),
					*Widget->GetName());

				if (auto* Confirm = Cast<UKRConfirmModal>(Widget))
				{
					UE_LOG(LogShopBuy, Log, TEXT("[ShopBuy] Cast to UKRConfirmModal success. Calling SetupConfirmWithQuantity"));

					Confirm->SetupConfirmWithQuantity(
						Msg,
						EConfirmContext::ShopBuy,
						ItemTag,
						1,
						99,
						1
					);

					UE_LOG(LogShopBuy, Log, TEXT("[ShopBuy] SetupConfirmWithQuantity finished"));
				}
				else
				{
					UE_LOG(LogShopBuy, Error, TEXT("[ShopBuy] Widget is not UKRConfirmModal! Class = %s"),
						*Widget->GetClass()->GetName());
				}
			}
			else
			{
				UE_LOG(LogShopBuy, Warning, TEXT("[ShopBuy] Router->ToggleRoute(\"Confirm\") returned nullptr"));
			}
		}
		else
		{
			UE_LOG(LogShopBuy, Error, TEXT("[ShopBuy] UKRUIRouterSubsystem not found on GameInstance"));
		}
	}
	else
	{
		UE_LOG(LogShopBuy, Error, TEXT("[ShopBuy] GameInstance is nullptr"));
	}
}

void UKRShopBuy::HandleMoveLeft()
{
	if (!ShoppingSlot)
	{
		return;
	}

	int32 Current = ShoppingSlot->GetSelectedIndex();
	int32 Columns = ShoppingSlot->GetColumnCount();
	int32 Total = ShoppingSlot->GetNumCells();

	int32 Next = StepGrid(Current, 0, Columns, Total);
	ShoppingSlot->SelectIndexSafe(Next);

	OnShopItemSelected(Next);
}

void UKRShopBuy::HandleMoveRight()
{
	if (!ShoppingSlot)
	{
		return;
	}

	int32 Current = ShoppingSlot->GetSelectedIndex();
	int32 Columns = ShoppingSlot->GetColumnCount();
	int32 Total = ShoppingSlot->GetNumCells();

	int32 Next = StepGrid(Current, 1, Columns, Total);
	ShoppingSlot->SelectIndexSafe(Next);

	OnShopItemSelected(Next);
}

void UKRShopBuy::HandleMoveUp()
{
	if (!ShoppingSlot)
	{
		return;
	}

	int32 Current = ShoppingSlot->GetSelectedIndex();
	int32 Columns = ShoppingSlot->GetColumnCount();
	int32 Total = ShoppingSlot->GetNumCells();

	int32 Next = StepGrid(Current, 2, Columns, Total);
	ShoppingSlot->SelectIndexSafe(Next);

	OnShopItemSelected(Next);
}

void UKRShopBuy::HandleMoveDown()
{
	if (!ShoppingSlot)
	{
		return;
	}

	int32 Current = ShoppingSlot->GetSelectedIndex();
	int32 Columns = ShoppingSlot->GetColumnCount();
	int32 Total = ShoppingSlot->GetNumCells();

	int32 Next = StepGrid(Current, 3, Columns, Total);
	ShoppingSlot->SelectIndexSafe(Next);

	OnShopItemSelected(Next);
}

int32 UKRShopBuy::StepGrid(int32 Current, int32 DirIndex, int32 NumColumns, int32 NumTotal) const
{
	if (NumTotal <= 0 || NumColumns <= 0)
	{
		return Current;
	}

	int32 Row = Current / NumColumns;
	int32 Col = Current % NumColumns;

	switch (DirIndex)
	{
	case 0:  // Left
		Col = (Col > 0) ? (Col - 1) : Col;
		break;
	case 1:  // Right
		Col = (Col < NumColumns - 1) ? (Col + 1) : Col;
		break;
	case 2:  // Up
		Row = (Row > 0) ? (Row - 1) : Row;
		break;
	case 3:  // Down
	{
		int32 MaxRow = (NumTotal - 1) / NumColumns;
		Row = (Row < MaxRow) ? (Row + 1) : Row;
		break;
	}
	default:
		break;
	}

	int32 Next = Row * NumColumns + Col;
	if (Next >= NumTotal) Next = NumTotal - 1;


	return FMath::Clamp(Next, 0, NumTotal - 1);
}

void UKRShopBuy::RefreshShopInventory()
{
	UKRUIAdapterLibrary::GetShopUIData(this, CachedShopItems);

	if (ShoppingSlot)
	{
		ShoppingSlot->InitializeItemGrid(CachedShopItems);
		if (CachedShopItems.Num() > 0)
		{
			UpdateItemDescription(0);
		}
		else
		{
			UpdateItemDescription(INDEX_NONE);
		}
	}
}

void UKRShopBuy::UpdatePlayerCurrency()
{
	if (!PlayerCurrencyPanel) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	AKRPlayerState* KRPS = PC->GetPlayerState<AKRPlayerState>();
	if (!KRPS) return;

	UKRCurrencyComponent* Currency = KRPS->GetCurrencyComponentSet();
	if (!Currency) return;

	Currency->ForceBroadcastCurrencyUI();
}


void UKRShopBuy::OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message)
{
	UpdatePlayerCurrency();
	RefreshShopInventory();
}
