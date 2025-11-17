// #pragma once
//
// #include "Engine/World.h"
// #include "KREquipmentInstance.generated.h"
//
// class AActor;
// class APawn;
// struct FFrame;
// struct FKREquipmentActorToSpawn;
//
// UCLASS(BlueprintType, Blueprintable)
// class KORAPROJECT_API UKREquipmentInstance : public UObject
// {
// 	GENERATED_BODY()
//
// public:
// 	UKREquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
// 	
// 	virtual UWorld* GetWorld() const override final;
//
// 	UFUNCTION(BlueprintPure, Category=Equipment)
// 	UObject* GetInstigator() const { return Instigator; }
//
// 	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }
//
// 	UFUNCTION(BlueprintPure, Category=Equipment)
// 	APawn* GetPawn() const;
//
// 	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
// 	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;
//
// 	UFUNCTION(BlueprintPure, Category=Equipment)
// 	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }
//
// 	virtual void SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn);
// 	virtual void DestroyEquipmentActors();
//
// 	virtual void OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn);
// 	virtual void OnUnequipped();
//
// private:
// 	UPROPERTY()
// 	TObjectPtr<UObject> Instigator;
//
// 	UPROPERTY()
// 	TArray<TObjectPtr<AActor>> SpawnedActors;
// };
