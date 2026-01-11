// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KRItemSlotBase.h"
#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "CommonNumericTextBlock.h"

void UKRItemSlotBase::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureStringTableLoaded();
}

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
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromLazyTexture(InData.ItemIcon, true);
		FSlateBrush Brush = ItemIcon->GetBrush();
		Brush.ImageSize = FVector2D(70.f, 70.f);
		ItemIcon->SetBrush(Brush);
	}
	if (ShopStock)			ShopStock->SetText(FText::AsNumber(InData.ShopStock));

	ApplyFieldsVisibility((OverrideFields >= 0) ? OverrideFields : VisibleFields);
}

void UKRItemSlotBase::ClearItemData()
{
	if (ItemName)			ItemName->SetText(FText::GetEmpty());
	if (ItemDescription)	ItemDescription->SetText(FText::GetEmpty());
	if (ItemCount)			ItemCount->SetText(FText::GetEmpty());
	if (ItemPrice)			ItemPrice->SetText(FText::GetEmpty());
	if (ItemIcon)			SetWidgetVisible(ItemIcon, false);
	if (ShopStock)			ShopStock->SetText(FText::GetEmpty());
}

void UKRItemSlotBase::SetHovered(bool bHovered)
{
	SetRenderOpacity(bHovered ? 0.8f : 1.0f);
}

void UKRItemSlotBase::ApplyFieldsVisibility(int32 Fields)
{
	SetWidgetVisible(ItemIcon, HasField(Fields, EKRItemField::Icon));
	SetWidgetVisible(ItemName, HasField(Fields, EKRItemField::Name));
	SetWidgetVisible(ItemDescription, HasField(Fields, EKRItemField::Description));
	SetWidgetVisible(ItemCount, HasField(Fields, EKRItemField::Count));
	SetWidgetVisible(ItemPrice, HasField(Fields, EKRItemField::Price));
	SetWidgetVisible(ShopStock, HasField(Fields, EKRItemField::ShopStock));
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
	const bool bNoShopStock = (D.ShopStock <= 0);
	return bNoName && bNoDesc && bNoIcon && bNoQty && bNoPrice && bNoShopStock;
}

void UKRItemSlotBase::EnsureStringTableLoaded()
{
	if (bStringTableLoaded)
	{
		return;
	}

	if (!ItemStringTableAsset.IsNull())
	{
		CachedStringTable = ItemStringTableAsset.LoadSynchronous();
	}

	bStringTableLoaded = true;
}

FText UKRItemSlotBase::GetSourceStringFromTable(const FName& InKey)
{
	if (InKey.IsNone())
	{
		return FText::GetEmpty();
	}

	EnsureStringTableLoaded();

	if (CachedStringTable)
	{
		FText Result = FText::FromStringTable(CachedStringTable->GetStringTableId(), InKey.ToString());
		if (!Result.IsEmpty())
		{
			return Result;
		}
	}

	return FText::FromName(InKey);
}