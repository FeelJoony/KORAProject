// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Shop/KRShopBuy.h"
#include "UI/KRSlotGridBase.h"
#include "UI/KRItemDescriptionBase.h"
#include "UI/Data/KRItemUIData.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "UI/Modal/KRConfirmModal.h"

#include "CommonTextBlock.h"
#include "CommonButtonBase.h"
#include "CommonNumericTextBlock.h"
#include "Engine/Texture2D.h"

void UKRShopBuy::RefreshShopInventory()
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

void UKRShopBuy::UpdatePlayerCurrency()
{

}

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

	if (auto* InputSubsys = GetOwningLocalPlayer()->GetSubsystem<UKRUIInputSubsystem>())
	{
		InputSubsys->UnbindAll(this);
	}
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
	if (!ShoppingSlot) return;

	const int32 Index = ShoppingSlot->GetSelectedIndex();
	if (!CachedShopItems.IsValidIndex(Index)) return;

	const FKRItemUIData& ItemData = CachedShopItems[Index];
	FGameplayTag ItemTag = ItemData.ItemTag;

	FText Msg = FText::FromStringTable(
		TEXT("/Game/UI/StringTable/ST_UIBaseTexts"),
		TEXT("Modal_BuyConfirm")
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
						EConfirmContext::ShopBuy,
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

void UKRShopBuy::OnCurrencyMessageReceived(FGameplayTag Channel, const FKRUIMessage_Currency& Message)
{
	UpdatePlayerCurrency();
	RefreshShopInventory(); // Needs Shop Inventory Updates
}
