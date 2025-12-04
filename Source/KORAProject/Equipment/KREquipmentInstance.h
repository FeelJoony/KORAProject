#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "KREquipmentInstance.generated.h"

class APawn;
class AActor;
struct FKREquipmentActorToSpawn;
struct FEquipmentDataStruct;

/**
 * Equipment Instance Base
 * 모든 장비의 기본 클래스 (무기, 방어구 등)
 */
UCLASS(Blueprintable, BlueprintType)
class KORAPROJECT_API UKREquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UKREquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UWorld* GetWorld() const override final;

	// Instigator (장착한 주체)
	UFUNCTION(BlueprintPure, Category = Equipment)
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category = Equipment)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = Equipment)
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	virtual void InitializeFromData(const FEquipmentDataStruct& InData);

	UFUNCTION()
	virtual void InitializeStats(const UInventoryFragment_SetStats* Stats) {}

	UFUNCTION()
	virtual void ApplyEnhanceLevel(int32 Level) {}
	
	// 장착된 Actor들
	UFUNCTION(BlueprintPure, Category = Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	// Actor 스폰/제거
	virtual void SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();

	// 장착/해제 이벤트
	virtual void OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn);
	virtual void OnUnequipped();

protected:
	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> SpawnedActors;

	//--------Config--------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	FGameplayTag ItemTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	FGameplayTagContainer CompatibleModuleSlots;

	//--------Visuals--------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visuals")
	TSoftObjectPtr<UStreamableRenderAsset> CachedMeshAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	FName AttachSocketName;

private:
	class UMeshComponent* CreateMeshComponent(AActor* InOwnerActor, UStreamableRenderAsset* InMeshAsset);

	void SpawnFromDefinition(APawn* InPawn, USceneComponent* InAttachTarget, const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn);
	
	void SpawnFromDataTable(APawn* InPawn, USceneComponent* InAttachTarget);
};