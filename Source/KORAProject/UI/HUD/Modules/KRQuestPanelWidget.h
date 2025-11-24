// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRQuestPanelWidget.generated.h"

class UCommonTextBlock;

UCLASS()
class KORAPROJECT_API UKRQuestPanelWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()
	
public:
	virtual void OnHUDInitialized() override;
	virtual void NativeDestruct() override;

protected:
	virtual void UnbindAll() override;
	void OnQuestMessageReceived(struct FGameplayTag Channel, const struct FKRUIMessage_Quest& Message);

	UFUNCTION(BlueprintImplementableEvent, Category = "Quest")
	void BP_OnUpdateQuestPanel(const FName QuestObjectiveKey);

private:
	FGameplayMessageListenerHandle QuestListener;
};
