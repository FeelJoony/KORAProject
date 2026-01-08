#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayTagContainer.h"
#include "KRGameplayCueNotify_CoreDriveSurge.generated.h"

class UKRStarDashData;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class KORAPROJECT_API AKRGameplayCueNotify_CoreDriveSurge : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:	
	AKRGameplayCueNotify_CoreDriveSurge();

protected:
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

	UPROPERTY(EditDefaultsOnly, Category = "CoreDrive|Data")
	TObjectPtr<UKRStarDashData> StarDashData;

private:
	void SpawnSplineLightning(AActor* Target, const FGameplayCueParameters& Parameters);
	void SpawnSlashLineEffect(AActor* Target, const FGameplayCueParameters& Parameters);
	void StartAfterImageEffect(AActor* Target);
	void StopAfterImageEffect();
	void SetCharacterOpacity(AActor* Target, float Opacity);
	void ResetCharacterOpacity(AActor* Target);

	UPROPERTY()
	TWeakObjectPtr<AActor> OwnerActor;
	
	UPROPERTY()
	TObjectPtr<AActor> SpawnedSplineLightningActor;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> SpawnedSlashEffectComponent;

	UPROPERTY()
	TObjectPtr<class UNiagaraComponent> SpawnedAfterImageComponent;

	UPROPERTY()
	TWeakObjectPtr<UMeshComponent> CachedMesh;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> CachedOriginalMaterials;
};
