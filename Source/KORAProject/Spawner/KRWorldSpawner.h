// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "KRWorldSpawner.generated.h"

USTRUCT(BlueprintType)
struct FKRSpawnData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<AActor> ActorClass; // 경로 깨짐 현상이 있으면 TSubclassOf로 변경

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform SpawnTransform;
};

UCLASS()
class KORAPROJECT_API AKRWorldSpawner : public AActor
{
	GENERATED_BODY()

public:
	AKRWorldSpawner();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void RespawnAll();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void ClearAllSpawned();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TArray<FKRSpawnData> SpawnDataList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	int32 SpawnsPerFrame = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	bool bSpawnOnBeginPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float FloorTraceHeight = 2000.f;

private:
	FVector FindFloorLocation(const FVector& InLocation);
	void SpawnNext();

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SpawnedActors;

	int32 CurrentSpawnIndex = 0;
	FTimerHandle SpawnTimerHandle;
};
