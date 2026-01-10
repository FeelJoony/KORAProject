// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawner/KRWorldSpawner.h"
#include "TimerManager.h"

AKRWorldSpawner::AKRWorldSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKRWorldSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay)
	{
		RespawnAll();
	}
}

void AKRWorldSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	ClearAllSpawned();
	Super::EndPlay(EndPlayReason);
}

void AKRWorldSpawner::RespawnAll()
{
	ClearAllSpawned();

	CurrentSpawnIndex = 0;
	SpawnedActors.Reserve(SpawnDataList.Num());

	if (SpawnDataList.Num() > 0)
	{
		GetWorldTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&AKRWorldSpawner::SpawnNext,
			0.001f,
			true
		);
	}
}

void AKRWorldSpawner::ClearAllSpawned()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

	for (TWeakObjectPtr<AActor>& WeakActor : SpawnedActors)
	{
		if (AActor* Actor = WeakActor.Get())
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Empty();
}

void AKRWorldSpawner::SpawnNext()
{
	if (!GetWorld())
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
		return;
	}

	int32 SpawnedThisFrame = 0;

	while (CurrentSpawnIndex < SpawnDataList.Num() && SpawnedThisFrame < SpawnsPerFrame)
	{
		const FKRSpawnData& Data = SpawnDataList[CurrentSpawnIndex];

		if (!Data.ActorClass.IsNull())
		{
			UClass* LoadedClass = Data.ActorClass.LoadSynchronous();
			if (LoadedClass)
			{
				FVector SpawnLocation = FindFloorLocation(Data.SpawnTransform.GetLocation());
				FTransform SpawnTransform = Data.SpawnTransform;
				SpawnTransform.SetLocation(SpawnLocation);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AActor* NewActor = GetWorld()->SpawnActor<AActor>(
					LoadedClass,
					SpawnTransform,
					SpawnParams
				);

				if (NewActor)
				{
					SpawnedActors.Add(NewActor);
				}
			}
		}

		CurrentSpawnIndex++;
		SpawnedThisFrame++;
	}

	if (CurrentSpawnIndex >= SpawnDataList.Num())
	{
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}
}

FVector AKRWorldSpawner::FindFloorLocation(const FVector& InLocation)
{
	if (!GetWorld())
	{
		return InLocation;
	}

	FVector Start = FVector(InLocation.X, InLocation.Y, InLocation.Z + FloorTraceHeight);
	FVector End = FVector(InLocation.X, InLocation.Y, InLocation.Z - FloorTraceHeight);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(this);

	for (const TWeakObjectPtr<AActor>& WeakActor : SpawnedActors)
	{
		if (AActor* Actor = WeakActor.Get())
		{
			QueryParams.AddIgnoredActor(Actor);
		}
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	if (GetWorld()->LineTraceSingleByObjectType(HitResult, Start, End, ObjectQueryParams, QueryParams))
	{
		return HitResult.Location;
	}

	return InLocation;
}
