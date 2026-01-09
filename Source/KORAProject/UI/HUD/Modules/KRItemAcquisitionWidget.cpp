// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/Modules/KRItemAcquisitionWidget.h"
#include "Engine/World.h"
#include "UI/Data/KRUIAdapterLibrary.h"

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
		FKRItemUIData UIData;
		if (UKRUIAdapterLibrary::GetItemUIDataByTag(this, Message.ItemTag, UIData))
		{
			BP_OnItemAcquired(UIData.ItemNameKey, Message.AcquiredQuantity);
		}
	}
}
