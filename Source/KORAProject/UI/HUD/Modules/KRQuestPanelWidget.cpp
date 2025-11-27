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
		Subsys.UnregisterListener(QuestListener);
	}
}

void UKRQuestPanelWidget::OnQuestMessageReceived(FGameplayTag Channel, const FKRUIMessage_Quest& Message)
{
	BP_OnUpdateQuestPanel(Message.QuestNameKey);
}
