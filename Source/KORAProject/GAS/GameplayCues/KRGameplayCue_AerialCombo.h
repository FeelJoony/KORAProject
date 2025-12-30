#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayTagContainer.h"
#include "KRGameplayCue_AerialCombo.generated.h"

UCLASS()
class KORAPROJECT_API UKRGameplayCue_AerialCombo : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UKRGameplayCue_AerialCombo();

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AerialCombo|Sound")
	FGameplayTag DefaultSoundTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AerialCombo|Effect")
	FGameplayTag DefaultEffectTag;
	
	FGameplayTag FindSoundTagFromParams(const FGameplayCueParameters& Parameters) const;
	FGameplayTag FindEffectTagFromParams(const FGameplayCueParameters& Parameters) const;
	
	void PlaySound(UWorld* World, const FGameplayTag& SoundTag, const FVector& Location, AActor* Target) const;
	void SpawnEffect(UWorld* World, const FGameplayTag& EffectTag, AActor* Target, const FVector& FallbackLocation) const;
};

UCLASS()
class KORAPROJECT_API UKRGameplayCue_AerialCombo_Launch : public UKRGameplayCue_AerialCombo
{
	GENERATED_BODY()

public:
	UKRGameplayCue_AerialCombo_Launch();
};

UCLASS()
class KORAPROJECT_API UKRGameplayCue_AerialCombo_Hit : public UKRGameplayCue_AerialCombo
{
	GENERATED_BODY()

public:
	UKRGameplayCue_AerialCombo_Hit();
};

UCLASS()
class KORAPROJECT_API UKRGameplayCue_AerialCombo_Finish : public UKRGameplayCue_AerialCombo
{
	GENERATED_BODY()

public:
	UKRGameplayCue_AerialCombo_Finish();

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AerialCombo|Finish")
	FGameplayTag GroundCrackEffectTag;
};
