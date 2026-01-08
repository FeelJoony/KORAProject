#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "TimerManager.h"
#include "KRGameplayCueNotify_CoreDriveBurstLoop.generated.h"

class UKRStarDashData;
class UNiagaraComponent;

/**
 * CoreDrive Burst 통합 연출 (투명도, 잔상, 라인, 번개)
 * LoopCueTag에 의해 호출됨
 */
UCLASS()
class KORAPROJECT_API AKRGameplayCueNotify_CoreDriveBurstLoop : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AKRGameplayCueNotify_CoreDriveBurstLoop();

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TObjectPtr<UKRStarDashData> StarDashData;

private:
	void SetCharacterOpacity(AActor* Target, float Opacity);
	void ResetCharacterOpacity(AActor* Target);
	
	void StartAfterImage(AActor* Target);
	void StopAfterImage();

	void StartEffectSequence();
	void PerformNextEffect();
	void SpawnLineEffect();
	void SpawnLightningEffect(const FVector& Start, const FVector& End);

	UPROPERTY()
	TWeakObjectPtr<AActor> OwnerActor;

	UPROPERTY()
	TWeakObjectPtr<USkeletalMeshComponent> CachedMesh;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> CachedOriginalMaterials;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SpawnedAfterImageComponent;

	// 시퀀스 처리용
	TArray<FVector> TargetPoints;
	FTimerHandle SequenceTimerHandle;
	int32 CurrentSequenceIndex = 0;
};
