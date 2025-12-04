// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "KREnemyHPWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;
class UKRCombatCommonSet;
struct FOnAttributeChangeData;

UCLASS()
class KORAPROJECT_API UKREnemyHPWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "EnemyHP")
	void InitFromASC(UAbilitySystemComponent* InASC, AActor* InOwnerActor);

	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget)) UProgressBar* MainHPBar = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* GreyHPBar = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* DamageAmount = nullptr;

	void UnbindFromASC();
	void BindToASC(UAbilitySystemComponent* InASC);

	void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);
	void InitBarsFromASC();

	UFUNCTION(BlueprintImplementableEvent, Category = "EnemyHP")
	void BP_OnTakeDamage(float DamageValue);

private:
	TWeakObjectPtr<UAbilitySystemComponent>    ASCWeak;
	TWeakObjectPtr<const UKRCombatCommonSet>   CombatAttrWeak;
	TWeakObjectPtr<AActor>                     OwnerActorWeak;

	FDelegateHandle HealthChangedHandle;

	float MainDisplayPercent = 1.f;
	float MainTargetPercent = 1.f;

	float GreyDisplayPercent = 1.f;
	float GreyTargetPercent = 1.f;
	UPROPERTY(EditAnywhere, Category = "EnemyHP|Anim") float GreyDelay = 0.4f;
	UPROPERTY(EditAnywhere, Category = "EnemyHP|Anim") float GreyAnimTime = 0.4f;

	float GreyElapsed = 0.f;
	bool  bGreyAnimating = false;
	bool  bGreyPending = false;

	FTimerHandle GreyDelayTimerHandle;
	FTimerHandle GreyAnimTimerHandle;

	void StartGreyAnim();
	void TickGreyAnim();
	void TickGreyHPBar(float NewValueNormalized);
};
