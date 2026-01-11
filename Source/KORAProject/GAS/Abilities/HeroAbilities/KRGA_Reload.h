#pragma once

#include "CoreMinimal.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "KRGA_Reload.generated.h"

class UGameplayEffect;

UCLASS()
class KORAPROJECT_API UKRGA_Reload : public UKRGameplayAbility
{
	GENERATED_BODY()

public:
	UKRGA_Reload(const FObjectInitializer& ObjectInitializer);

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	TSubclassOf<UGameplayEffect> ReloadEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Reload|Sound")
	FGameplayTag ReloadStartTag = FGameplayTag::RequestGameplayTag(FName("Sound.Combat.Gun.Reload.Start"));

	UPROPERTY(EditDefaultsOnly, Category = "Reload|Sound")
	FGameplayTag ReloadLoadTag = FGameplayTag::RequestGameplayTag(FName("Sound.Combat.Gun.Reload.Loop"));

	UPROPERTY(EditDefaultsOnly, Category = "Reload|Sound")
	FGameplayTag ReloadEndTag = FGameplayTag::RequestGameplayTag(FName("Sound.Combat.Gun.Reload.End"));
	
	UPROPERTY(EditDefaultsOnly, Category = "Reload|Time")
	float Delay_Start = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Reload|Time")
	float Delay_Load = 0.5f;

private:
	void Step1_StartReload();

	UFUNCTION()
	void Step2_LoadReload();

	UFUNCTION()
	void Step3_EndReload();
	
	void PlaySoundByTag(FGameplayTag Tag);
};