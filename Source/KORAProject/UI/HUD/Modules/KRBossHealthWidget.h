// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRBossHealthWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;

UCLASS()
class KORAPROJECT_API UKRBossHealthWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void SetBossASC(UAbilitySystemComponent* InASC);

	virtual void OnHUDInitialized() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget)) UProgressBar* BossHP = nullptr;

	virtual void UnbindAll() override;

private:
	TWeakObjectPtr<UAbilitySystemComponent> BossASCWeak;
	FGameplayMessageListenerHandle BossListenerHandle;

	float BossDisplayPercent = 1.f;
	float BossTargetPercent = 1.f;
	float BossAnimTime = 0.3f;
	float BossAnimElapsed = 0.f;
	FTimerHandle BossAnimTimerHandle;

	void OnBossMessage(FGameplayTag ChannelTag, const FKRUIMessage_Progress& Message);
	bool IsMessageFromBoss(const TWeakObjectPtr<AActor>& TargetActor) const;

	void StartBossAnim();
	void TickBossAnim();
};