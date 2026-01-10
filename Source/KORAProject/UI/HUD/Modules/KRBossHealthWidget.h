// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRBossHealthWidget.generated.h"

class UProgressBar;
class UCommonTextBlock;
class UAbilitySystemComponent;
struct FOnAttributeChangeData;
class UKRCombatCommonSet;

UCLASS()
class KORAPROJECT_API UKRBossHealthWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()

public:
	virtual void OnHUDInitialized() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget)) UCommonTextBlock* BossName = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* BossHP = nullptr;

	virtual void UnbindAll() override;

private:
	TWeakObjectPtr<UAbilitySystemComponent>      BossASCWeak;
	TWeakObjectPtr<const UKRCombatCommonSet>     BossCombatAttr;

	FDelegateHandle BossHealthChangedHandle;

	float BossDisplayPercent = 1.f;
	float BossTargetPercent = 1.f;
	float BossAnimTime = 0.3f;
	float BossAnimElapsed = 0.f;
	FTimerHandle BossAnimTimerHandle;

	FGameplayMessageListenerHandle BossMessageListenerHandle;

	void BindToBossASC(UAbilitySystemComponent* InASC);
	void InitBossBarFromASC();

	void OnBossHealthAttributeChanged(const FOnAttributeChangeData& Data);

	void StartBossAnim();
	void TickBossAnim();

	void OnBossMessageReceived(FGameplayTag Channel, const FKRUIMessage_Boss& Message);
	void SetBossASC(UAbilitySystemComponent* InASC, const FName& InBossNameKey);
};