// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KRItemDescriptionBase.h"
#include "Internationalization/StringTableRegistry.h"

const FName UKRItemDescriptionBase::ItemStringTableId(TEXT("ST_ItemList"));

FText UKRItemDescriptionBase::GetLocalizedTextFromKey(const FName& InKey) const
{
	if (InKey.IsNone())
	{
		return FText::GetEmpty();
	}

	FStringTableConstPtr StringTable = FStringTableRegistry::Get().FindStringTable(ItemStringTableId);
	if (!StringTable.IsValid())
	{
		return FText::FromName(InKey);
	}

	FText Result = FText::FromStringTable(ItemStringTableId, InKey.ToString());

	if (Result.IsEmpty())
	{
		return FText::FromName(InKey);
	}

	return Result;
}
