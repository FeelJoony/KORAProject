// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRLevelDeathSaveInfo.generated.h"

class UCommonTextBlock;
class UStringTable;

UCLASS()
class KORAPROJECT_API UKRLevelDeathSaveInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> InfoText;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowAnim;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> HideAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelDeathSaveInfo")
	float AutoHideDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelDeathSaveInfo|Keys")
	FString SaveGameKey = TEXT("Info_SaveGame");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelDeathSaveInfo|Keys")
	FString DeathKey = TEXT("Info_Death");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LevelDeathSaveInfo")
	TSoftObjectPtr<UStringTable> StringTableAsset;

private:
	void HandleInfoMessage(FGameplayTag Channel, const FKRUIMessage_Info& Message);
	void UpdateInfoText(const FString& StringTableKey);
	void ShowInfo(EInfoContext Context, const FString& StringTableKey);
	void HideInfo();
	void OnAutoHideTimeout();

	FGameplayMessageListenerHandle InfoMessageListener;
	FTimerHandle AutoHideTimerHandle;
	bool bIsShowing = false;
};
