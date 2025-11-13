// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/HUD/KRHUDWidgetBase.h"
#include "KRBossHealthWidget.generated.h"

class UProgressBar; 
class UKRAttributeSet;

UCLASS()
class KORAPROJECT_API UKRBossHealthWidget : public UKRHUDWidgetBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable) void SetBossASC(UAbilitySystemComponent* InASC);

protected:
	void UnbindBoss();
	void RefreshBoss();
	//void OnBossAttrChanged( InData);

	UPROPERTY(meta = (BindWidget)) UProgressBar* BossHP = nullptr;
	TWeakObjectPtr<UAbilitySystemComponent> BossASC;
	FDelegateHandle H_BossHP, H_BossMax;
};
