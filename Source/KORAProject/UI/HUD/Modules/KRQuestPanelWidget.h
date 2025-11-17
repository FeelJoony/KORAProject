// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "KRQuestPanelWidget.generated.h"

class UCommonTextBlock;

UCLASS()
class KORAPROJECT_API UKRQuestPanelWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget)) UCommonTextBlock* Objective = nullptr;

	UFUNCTION(BlueprintCallable) void UpdateQuestPanel(FText InObjective);
};
