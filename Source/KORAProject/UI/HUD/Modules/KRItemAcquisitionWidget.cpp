// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/Modules/KRItemAcquisitionWidget.h"
#include "Engine/World.h"
#include "UI/Data/KRUIAdapterLibrary.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/CurrencyDataStruct.h"

void UKRItemAcquisitionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

		ItemLogListener = Subsys.RegisterListener(
			FKRUIMessageTags::ItemLog(),
			this,
			&ThisClass::OnItemLogMessageReceived
		);
	}
}

void UKRItemAcquisitionWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		Subsys.UnregisterListener(ItemLogListener);
	}

	Super::NativeDestruct();
}

void UKRItemAcquisitionWidget::OnItemLogMessageReceived(FGameplayTag Channel, const FKRUIMessage_ItemLog& Message)
{
	if (Message.EventType == EItemLogEvent::AcquiredNormal || Message.EventType == EItemLogEvent::AcquiredQuest)
	{
		if (Message.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Currency"))))
		{
			FName CurrencyDisplayName = NAME_None;
			if (UKRDataTablesSubsystem* DataSubsystem = UKRDataTablesSubsystem::GetSafe(this))
			{
				if (UCacheDataTable* CurrencyTable = DataSubsystem->GetTable(FCurrencyDataStruct::StaticStruct()))
				{
					if (UDataTable* DT = CurrencyTable->GetTable())
					{
						TArray<FCurrencyDataStruct*> AllRows;
						DT->GetAllRows(TEXT("CurrencyLookup"), AllRows);
						for (const FCurrencyDataStruct* Row : AllRows)
						{
							if (Row && Row->CurrencyTag == Message.ItemTag)
							{
								CurrencyDisplayName = Row->CurrencyID;
								break;
							}
						}
					}
				}
			}

			if (CurrencyDisplayName == NAME_None)
			{
				CurrencyDisplayName = Message.ItemTag.GetTagName();
			}
			BP_OnItemAcquired(CurrencyDisplayName, Message.AcquiredQuantity);
			return;
		}

		FKRItemUIData UIData;
		if (UKRUIAdapterLibrary::GetItemUIDataByTag(this, Message.ItemTag, UIData))
		{
			BP_OnItemAcquired(UIData.ItemNameKey, Message.AcquiredQuantity);
		}
	}
}
