// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "UI/Data/UIStruct/KRProgressBarMessages.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRPlayerStatusWidget.generated.h"

class UProgressBar; 
class UKRAttributeSet;

UCLASS()
class KORAPROJECT_API UKRPlayerStatusWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget)) UProgressBar* MainHPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* GreyHPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* StaminaBar = nullptr;

	// UUswerWidget 기반으로 코어 드라이브 에너지를 Percentage를 받아서 하나씩 채우는 게 있는데,
	// 이 경우 총 3개의 Energy 바가 있고, 1번 부터 차곡차곡 채워짐

	// 현재 들고 있는 무기가 검인지 총인지를 받아서 BP에서 애니메이션 처리 할 예정 

	virtual void OnHUDInitialized() override;
	virtual void NativeDestruct() override;
	virtual void UnbindAll() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "ProgressBar")
	void BP_OnCoreDriveChanged(int32 FullSegments, float PartialSegmentPercent);

private:
	FGameplayMessageListenerHandle PBMessageListener;

	float HealthDisplayPercent = 1.f;
	float HealthTargetPercent = 1.f;
	float HealthAnimTime = 0.3f;
	float HealthAnimElapsed = 0.f;
	FTimerHandle* HealthAnimTimerHandle = nullptr;

	float StaminaDisplayPercent = 1.f;
	float StaminaTargetPercent = 1.f;
	float StaminaAnimTime = 0.3f;
	float StaminaAnimElapsed = 0.f;
	FTimerHandle* StaminaAnimTimerHandle = nullptr;

	float CoreDriveValue = 0.f;
	float CoreDriveMax = 60.f;

	void ProcessMessage(FGameplayTag InTag, const FKRProgressBarMessages& Message);
	bool IsMessageFromMyActor(const FKRProgressBarMessages& Message) const;

	void StartHealthAnim();
	void TickHealthAnim();

	void StartStaminaAnim();
	void TickStaminaAnim();
};
