// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRQuestPanelWidget.h"
#include "Engine/World.h"

void UKRQuestPanelWidget::OnHUDInitialized()
{
	Super::OnHUDInitialized();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);

		QuestListener = Subsys.RegisterListener(
			FKRUIMessageTags::Quest(),
			this,
			&ThisClass::OnQuestMessageReceived
		);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UKRQuestPanelWidget::NativeDestruct()
{
	UnbindAll();
	Super::NativeDestruct();
}

void UKRQuestPanelWidget::UnbindAll()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsys = UGameplayMessageSubsystem::Get(World);
		if (QuestListener.IsValid())
		{
			Subsys.UnregisterListener(QuestListener);
		}
	}
}

void UKRQuestPanelWidget::OnQuestMessageReceived(FGameplayTag Channel, const FKRUIMessage_Quest& Message)
{
	SetVisibility(ESlateVisibility::Visible);
	BP_OnUpdateQuestPanel(Message.QuestNameKey);
}
