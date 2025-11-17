// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KRItemSlotBase.h"
#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "CommonNumericTextBlock.h"

const FName UKRItemSlotBase::ItemStringTableId(TEXT("/Game/UI/StringTable/ST_ItemList"));

void UKRItemSlotBase::SetItemData(const FKRItemUIData& InData, int32 OverrideFields)
{
	if (IsDataEmpty(InData))
	{
		ClearItemData();
		SetIsEnabled(false);
		return;
	}

	if (ItemName)			ItemName->SetText(GetSourceStringFromTable(InData.ItemNameKey));
	if (ItemDescription)	ItemDescription->SetText(GetSourceStringFromTable(InData.ItemDescriptionKey));
    if (ItemCount)			ItemCount->SetText(FText::AsNumber(InData.Quantity));
	if (ItemPrice)			ItemPrice->SetText(FText::AsNumber(InData.Price));
	if (ItemIcon)			ItemIcon->SetBrushFromLazyTexture(InData.ItemIcon, true);

	ApplyFieldsVisibility((OverrideFields >= 0) ? OverrideFields : VisibleFields);
}

void UKRItemSlotBase::ClearItemData()
{
	if (ItemName)			ItemName->SetText(FText::GetEmpty());
	if (ItemDescription)	ItemDescription->SetText(FText::GetEmpty());
	if (ItemCount)			ItemCount->SetText(FText::GetEmpty());
	if (ItemPrice)			ItemPrice->SetText(FText::GetEmpty());
	if (ItemIcon)			SetWidgetVisible(ItemIcon, false);
}

void UKRItemSlotBase::ApplyFieldsVisibility(int32 Fields)
{
	SetWidgetVisible(ItemIcon, HasField(Fields, EKRItemField::Icon));
	SetWidgetVisible(ItemName, HasField(Fields, EKRItemField::Name));
	SetWidgetVisible(ItemDescription, HasField(Fields, EKRItemField::Description));
	SetWidgetVisible(ItemCount, HasField(Fields, EKRItemField::Count));
	SetWidgetVisible(ItemPrice, HasField(Fields, EKRItemField::Price));
}

void UKRItemSlotBase::SetWidgetVisible(UWidget* W, bool bShow)
{
	if (W)
	{
		W->SetVisibility(bShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

bool UKRItemSlotBase::IsDataEmpty(const FKRItemUIData& D)
{
	const bool bNoName = D.ItemNameKey.IsNone();
	const bool bNoDesc = D.ItemDescriptionKey.IsNone();
	const bool bNoIcon = D.ItemIcon.IsNull();
	const bool bNoQty = (D.Quantity <= 0);
	const bool bNoPrice = (D.Price <= -1);
	return bNoName && bNoDesc && bNoIcon && bNoQty && bNoPrice;
}

FText UKRItemSlotBase::GetSourceStringFromTable(const FName& InKey)
{
	if (InKey.IsNone())
	{
		return FText::GetEmpty();
	}
	return FText::FromStringTable(ItemStringTableId, InKey.ToString());
}