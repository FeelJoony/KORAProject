// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRBossHealthWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;
struct FKRProgressBarMessages;

UCLASS()
class KORAPROJECT_API UKRBossHealthWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetBossASC(UAbilitySystemComponent* InASC);

protected:
	UPROPERTY(meta = (BindWidget)) UProgressBar* BossHP = nullptr;

	virtual void OnHUDInitialized() override;
	virtual void NativeDestruct() override;
	virtual void UnbindAll() override;

private:
	TWeakObjectPtr<UAbilitySystemComponent> BossASCWeak;
	FGameplayMessageListenerHandle BossListenerHandle;

	float BossDisplayPercent = 1.f;
	float BossTargetPercent = 1.f;
	float BossAnimTime = 0.3f;
	float BossAnimElapsed = 0.f;
	FTimerHandle BossAnimTimerHandle;

	void OnBossMessage(FGameplayTag ChannelTag, const FKRProgressBarMessages& Message);
	bool IsMessageFromBoss(const FKRProgressBarMessages& Message) const;

	void StartBossAnim();
	void TickBossAnim();
};