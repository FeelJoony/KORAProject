// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRPlayerStatusWidget.generated.h"

class UProgressBar;
class UWidget;
class UCommonTextBlock;

class UKRCombatCommonSet;
class UKRPlayerAttributeSet;
class UAbilitySystemComponent;
struct FOnAttributeChangeData;

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

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UWidget> GunAmmoSection = nullptr;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UCommonTextBlock> CurrentBullet;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UCommonTextBlock> MaxBullet;

	void BindToASC();
	void InitBarsFromASC();

	UFUNCTION(BlueprintImplementableEvent, Category = "ProgressBar")
	void BP_OnCoreDriveChanged(int32 FullSegments, float PartialSegmentPercent);
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerStatus", meta = (DisplayName = "On Weapon Changed"))
	void BP_OnWeaponChanged(const FGameplayTag& WeaponTypeTag);
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerStatus")
	void BP_OnGuardSuccess();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerStatus")
	void BP_OnPerfectGuardSuccess();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerStatus")
	void BP_OnStaminaDepleted();
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerStatus")
	void BP_OnCoreDriveSegmentFilled(int32 NewFullSegments);
	UFUNCTION(BlueprintImplementableEvent, Category = "PlayerStatus")
	void BP_OnCoreDriveFull();

	void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
	void OnGreyHealthAttributeChanged(const FOnAttributeChangeData& Data);
	void OnStaminaAttributeChanged(const FOnAttributeChangeData& Data);
	void OnCoreDriveAttributeChanged(const FOnAttributeChangeData& Data);

	void OnWeaponMessageReceived(FGameplayTag Channel, const FKRUIMessage_Weapon& Message);
	void OnGuardMessageReceived(FGameplayTag Channel, const FKRUIMessage_Guard& Message);

private:
	TWeakObjectPtr<UAbilitySystemComponent>     CachedASC;
	TWeakObjectPtr<const UKRCombatCommonSet>    CombatAttr;   // CurrentHealth / MaxHealth
	TWeakObjectPtr<const UKRPlayerAttributeSet> PlayerAttr;	// Stamina / CoreDrive

	FDelegateHandle HealthChangedHandle;
	FDelegateHandle GreyHPChangedHandle;
	FDelegateHandle StaminaChangedHandle;
	FDelegateHandle CoreDriveChangedHandle;

	FGameplayMessageListenerHandle WeaponMessageHandle;
	FGameplayMessageListenerHandle GuardMessageHandle;

	float HealthDisplayPercent = 1.f;
	float HealthTargetPercent = 1.f;
	float HealthAnimTime = 0.3f;
	float HealthAnimElapsed = 0.f;
	FTimerHandle HealthAnimTimerHandle;

	float GreyHPDisplayPercent = 1.f;
	float GreyHPTargetPercent = 1.f;
	float GreyHPAnimTime = 0.4f;
	float GreyHPAnimElapsed = 0.f;
	FTimerHandle GreyHPAnimTimerHandle;

	float StaminaDisplayPercent = 1.f;
	float StaminaTargetPercent = 1.f;
	float StaminaAnimTime = 0.3f;
	float StaminaAnimElapsed = 0.f;
	FTimerHandle StaminaAnimTimerHandle;

	float CoreDriveValue = 0.f;
	float CoreDriveMax = 60.f;

	int32 CoreDriveFullSegmentsPrev = 0;
	bool bIsCoreDriveFull = false;

	bool IsMessageFromMyActor(const TWeakObjectPtr<AActor>& TargetActor) const;

	void StartHealthAnim();
	void TickHealthAnim();

	void StartGreyHPAnim();
	void TickGreyHPAnim();

	void StartStaminaAnim();
	void TickStaminaAnim();

	void UpdateGreyHPBar(float NewValueNormalized);
	void UpdateWeaponAmmoDisplay(const FGameplayTag& WeaponTypeTag, int32 CurrentAmmo, int32 MaxAmmo);
};