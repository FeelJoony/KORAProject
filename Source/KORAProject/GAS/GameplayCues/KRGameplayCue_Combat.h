#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayTagContainer.h"
#include "KRGameplayCue_Combat.generated.h"

/**
 * 전투 GameplayCue - Sound/Effect 재생
 *
 * 사용 방법:
 * 1. BP로 상속 후 SoundTag/EffectTag settings
 * 2. or GA: Parameters.AggregatedSourceTags에 태그 전달
 *
 * GA:
 *   FGameplayCueParameters Params;
 *   Params.Location = HitLocation;
 *   Params.AggregatedSourceTags.AddTag(SoundTag);
 *   Params.AggregatedSourceTags.AddTag(EffectTag);
 *   ASC->ExecuteGameplayCue(GameplayCueTag, Params);
 */
UCLASS()
class KORAPROJECT_API UKRGameplayCue_Combat : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UKRGameplayCue_Combat();

	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Sound")
	FGameplayTag DefaultSoundTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effect")
	FGameplayTag DefaultEffectTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effect")
	FName DefaultEffectSocketName = NAME_None;

	// Sound 태그 필터 (이 태그의 자식 태그를 Sound로 인식)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Filter")
	FGameplayTag SoundTagFilter;

	// Effect 태그 필터 (이 태그의 자식 태그를 Effect로 인식)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Filter")
	FGameplayTag EffectTagFilter;

private:
	FGameplayTag FindSoundTagFromParams(const FGameplayCueParameters& Parameters) const;
	FGameplayTag FindEffectTagFromParams(const FGameplayCueParameters& Parameters) const;
};
