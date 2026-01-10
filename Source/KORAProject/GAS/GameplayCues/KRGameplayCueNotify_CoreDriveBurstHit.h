#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "KRGameplayCueNotify_CoreDriveBurstHit.generated.h"

class UKRStarDashData;

/**
 * CoreDrive Burst 스킬의 타격 연출 (Line, Lightning) 담당
 * BurstCueTag에 의해 호출됨
 * Static GCN으로 변경됨 (가벼운 즉발성 연출)
 */
UCLASS()
class KORAPROJECT_API UKRGameplayCueNotify_CoreDriveBurstHit : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UKRGameplayCueNotify_CoreDriveBurstHit();

	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;

private:
	void SpawnLineEffect(AActor* Target, const FGameplayCueParameters& Parameters, const UKRStarDashData* Data) const;
	void SpawnLightningEffect(AActor* Target, const FGameplayCueParameters& Parameters, const UKRStarDashData* Data) const;
};
