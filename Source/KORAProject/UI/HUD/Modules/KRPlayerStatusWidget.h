// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRPlayerStatusWidget.generated.h"

class UProgressBar;
class UKRAttributeSet;
class UImage;
class UTextBlock;

UCLASS()
class KORAPROJECT_API UKRPlayerStatusWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()

public:
	virtual void OnHUDInitialized() override;
	virtual void NativeDestruct() override;

protected:
	virtual void UnbindAll() override;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> MainHPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> GreyHPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> StaminaBar = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category = "ProgressBar")
	void BP_OnCoreDriveChanged(int32 FullSegments, float PartialSegmentPercent);
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerStatus", meta = (DisplayName = "On Weapon Changed"))
	void BP_OnWeaponChanged(const FKRUIMessage_Weapon& WeaponData);

	void OnProgressMessageReceived(FGameplayTag Channel, const FKRUIMessage_Progress& Message);
	void OnGreyHPMessageReceived(FGameplayTag Channel, const FKRUIMessage_GreyHP& Message);
	void OnWeaponMessageReceived(FGameplayTag Channel, const FKRUIMessage_Weapon& Message);

	UPROPERTY(EditDefaultsOnly, Category = "PlayerStatus|GreyHP") float GreyHPDecayDuration = 1.0f;

private:
	FGameplayMessageListenerHandle ProgressMessageHandle;
	FGameplayMessageListenerHandle GreyHPMessageHandle;
	FGameplayMessageListenerHandle WeaponMessageHandle;

	float HealthDisplayPercent = 1.f;
	float HealthTargetPercent = 1.f;
	float HealthAnimTime = 0.3f;
	float HealthAnimElapsed = 0.f;
	FTimerHandle HealthAnimTimerHandle;

	float StaminaDisplayPercent = 1.f;
	float StaminaTargetPercent = 1.f;
	float StaminaAnimTime = 0.3f;
	float StaminaAnimElapsed = 0.f;
	FTimerHandle StaminaAnimTimerHandle;

	float CoreDriveValue = 0.f;
	float CoreDriveMax = 60.f;

	float CurrentGreyHP = 0.f;
	float GreyHPDecayStartValue = 0.f;
	float GreyHPDecayElapsed = 0.f;
	FTimerHandle GreyHPDecayTimerHandle;


	bool IsMessageFromMyActor(const TWeakObjectPtr<AActor>& TargetActor) const;

	void StartHealthAnim();
	void TickHealthAnim();

	void StartStaminaAnim();
	void TickStaminaAnim();

	void RecoverGreyHP(float RecoveredAmount);
	void DecayGreyHP();
	void TickGreyHPDecayAnimation();

	void UpdateGreyHPBar(float NewValueNormalized);
};