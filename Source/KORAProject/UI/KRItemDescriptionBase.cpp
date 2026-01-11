// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KRItemDescriptionBase.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableCore.h"

void UKRItemDescriptionBase::NativeConstruct()
{
	Super::NativeConstruct();
	EnsureStringTableLoaded();
}

void UKRItemDescriptionBase::EnsureStringTableLoaded()
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

FText UKRItemDescriptionBase::GetLocalizedTextFromKey(const FName& InKey)
{
	if (InKey.IsNone())
	{
		return FText::GetEmpty();
	}

	EnsureStringTableLoaded();

	if (CachedStringTable)
	{
		// Check if the key exists in the StringTable before getting the text
		const FStringTable& RawTable = CachedStringTable->GetStringTable().Get();
		FStringTableEntryConstPtr Entry = RawTable.FindEntry(InKey.ToString());
		if (Entry.IsValid())
		{
			return FText::FromStringTable(CachedStringTable->GetStringTableId(), InKey.ToString());
		}
	}

	// Fallback: return the key itself as display text
	return FText::FromName(InKey);
}
