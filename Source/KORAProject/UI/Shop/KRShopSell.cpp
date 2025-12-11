// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Shop/KRShopSell.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "GameplayTag/KRItemTypeTag.h"
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

void UKRShopSell::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->BindBackDefault(this, TEXT("ShopSell"));

		InputSubsys->BindRow(this, TEXT("Prev"), FSimpleDelegate::CreateUObject(this, &UKRShopSell::HandleMoveLeft));
		InputSubsys->BindRow(this, TEXT("Next"), FSimpleDelegate::CreateUObject(this, &UKRShopSell::HandleMoveRight));
		InputSubsys->BindRow(this, TEXT("Increase"), FSimpleDelegate::CreateUObject(this, &UKRShopSell::HandleMoveUp));
		InputSubsys->BindRow(this, TEXT("Decrease"), FSimpleDelegate::CreateUObject(this, &UKRShopSell::HandleMoveDown));
		InputSubsys->BindRow(this, TEXT("Select"), FSimpleDelegate::CreateUObject(this, &UKRShopSell::HandleSelect));
	}
}

void UKRShopSell::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void UKRShopSell::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConsumablesButton) ConsumablesButton->OnClicked().AddUObject(this, &UKRShopSell::OnClickConsumables);
	if (MaterialButton) MaterialButton->OnClicked().AddUObject(this, &UKRShopSell::OnClickMaterial);

	OnClickConsumables();
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

void UKRShopSell::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		Subsys.UnregisterListener(CurrencyListener);
	}

	Super::NativeDestruct();
}

void UKRShopSell::FilterSellableItems(const FGameplayTag& FilterTag)
{
	CurrentFilterTag = FilterTag;

	UKRUIAdapterLibrary::GetInventoryUIDataFiltered(this, FilterTag, CachedShopItems);
	RefreshShopInventory();
}

void UKRShopSell::OnPlayerItemSelected(int32 SelectedIndex)
{

	UpdateItemDescription(SelectedIndex);
}

void UKRShopSell::UpdateItemDescription(int32 CellIndex)
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

void UKRShopSell::HandleSelect()
{
	if (!ShoppingSlot) return;

	const int32 Index = ShoppingSlot->GetSelectedIndex();
	if (!CachedShopItems.IsValidIndex(Index)) return;

	const FKRItemUIData& ItemData = CachedShopItems[Index];
	FGameplayTag ItemTag = ItemData.ItemTag;

	FText Msg = FText::FromStringTable(
		TEXT("/Game/UI/StringTable/ST_UIBaseTexts"),
		TEXT("Modal_SellConfirm")
	);
	if (UGameInstance* GI = GetGameInstance())
	{
		if (auto* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			if (auto* Widget = Router->ToggleRoute(TEXT("Confirm")))
			{
				if (auto* Confirm = Cast<UKRConfirmModal>(Widget))
				{
					Confirm->SetupConfirmWithQuantity(
						Msg,
						EConfirmContext::ShopSell,
						ItemTag,
						1,
						99,
						1
					);
				}
			}
		}
	}
}

void UKRShopSell::HandleMoveLeft()
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

	OnPlayerItemSelected(Next);
}

void UKRShopSell::HandleMoveRight()
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

	OnPlayerItemSelected(Next);
}

void UKRShopSell::HandleMoveUp()
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

	OnPlayerItemSelected(Next);
}

void UKRShopSell::HandleMoveDown()
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

	OnPlayerItemSelected(Next);
}

int32 UKRShopSell::StepGrid(int32 Current, int32 DirIndex, int32 NumColumns, int32 NumTotal) const
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

void UKRShopSell::RefreshShopInventory()
{
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

void UKRShopSell::UpdatePlayerCurrency()
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

void UKRShopSell::OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message)
{
	UpdatePlayerCurrency();

	if (CurrentFilterTag.IsValid()) FilterSellableItems(CurrentFilterTag);
	else FilterSellableItems(KRTAG_ITEMTYPE_CONSUME);
}

void UKRShopSell::OnClickConsumables()
{
	FilterSellableItems(KRTAG_ITEMTYPE_CONSUME);
}

void UKRShopSell::OnClickMaterial()
{
	FilterSellableItems(KRTAG_ITEMTYPE_MATERIAL);
}
