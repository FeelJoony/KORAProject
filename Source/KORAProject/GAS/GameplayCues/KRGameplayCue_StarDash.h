#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayTagContainer.h"
#include "KRGameplayCue_StarDash.generated.h"

UCLASS()
class KORAPROJECT_API UKRGameplayCue_StarDash : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UKRGameplayCue_StarDash();

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StarDash|Sound")
	FGameplayTag DefaultSoundTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StarDash|Effect")
	FGameplayTag DefaultEffectTag;

	FGameplayTag FindSoundTagFromParams(const FGameplayCueParameters& Parameters) const;
	FGameplayTag FindEffectTagFromParams(const FGameplayCueParameters& Parameters) const;
	
	void PlaySound(UWorld* World, const FGameplayTag& SoundTag, const FVector& Location, AActor* Target) const;
	void SpawnEffect(UWorld* World, const FGameplayTag& EffectTag, AActor* Target, const FVector& FallbackLocation) const;
};

UCLASS()
class KORAPROJECT_API UKRGameplayCue_StarDash_Dash : public UKRGameplayCue_StarDash
{
	GENERATED_BODY()

public:
	UKRGameplayCue_StarDash_Dash();
};


UCLASS()
class KORAPROJECT_API UKRGameplayCue_StarDash_Slash : public UKRGameplayCue_StarDash
{
	GENERATED_BODY()

public:
	UKRGameplayCue_StarDash_Slash();
};

UCLASS()
class KORAPROJECT_API UKRGameplayCue_StarDash_Finish : public UKRGameplayCue_StarDash
{
	GENERATED_BODY()

public:
	UKRGameplayCue_StarDash_Finish();
};
