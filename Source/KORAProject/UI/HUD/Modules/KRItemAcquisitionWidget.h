// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRItemAcquisitionWidget.generated.h"

UCLASS()
class KORAPROJECT_API UKRItemAcquisitionWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	void OnItemLogMessageReceived(FGameplayTag Channel, const FKRUIMessage_ItemLog& Message);

	UFUNCTION(BlueprintImplementableEvent, Category = "ItemAcquisition", meta = (DisplayName = "On Item Acquired"))
	void BP_OnItemAcquired(FName ItemNameKey, int32 ItemCount);

private:
	FGameplayMessageListenerHandle ItemLogListener;
};
