#include "Equipment/KREquipmentInstance.h"
#include "Equipment/KREquipmentDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "Data/EquipmentDataStruct.h"
#include "Components/MeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentInstance)

UKREquipmentInstance::UKREquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* UKREquipmentInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

APawn* UKREquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UKREquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void UKREquipmentInstance::InitializeFromData(const FEquipmentDataStruct& InData)
{
	ItemTag = InData.ItemTag;
	CompatibleModuleSlots = InData.CompatibleModuleSlots;
	CachedMeshAsset = InData.EquipmentMesh;
	AttachSocketName = InData.AttachSocketName;
}


void UKREquipmentInstance::SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
{
	APawn* OwningPawn = GetPawn();
	if (!OwningPawn) return;
	
	USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
	if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
	{
		AttachTarget = Char->GetMesh();
	}

	if (!ActorsToSpawn.IsEmpty())
	{
		SpawnFromDefinition(OwningPawn, AttachTarget, ActorsToSpawn);
	}
	else
	{
		SpawnFromDataTable(OwningPawn, AttachTarget);
	}
}

void UKREquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Empty();
}

void UKREquipmentInstance::OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
{
	SpawnEquipmentActors(ActorsToSpawn);
}

void UKREquipmentInstance::OnUnequipped()
{
	DestroyEquipmentActors();
}

void UKREquipmentInstance::SpawnFromDefinition(APawn* InPawn, USceneComponent* InAttachTarget, const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
{
	for (const FKREquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
	{
		AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, InPawn);
		if (!NewActor) continue;

		NewActor->FinishSpawning(FTransform::Identity, true);
		NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
		NewActor->AttachToComponent(InAttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);
		
		SpawnedActors.Add(NewActor);
	}
}

void UKREquipmentInstance::SpawnFromDataTable(APawn* InPawn, USceneComponent* InAttachTarget)
{
	if (CachedMeshAsset.IsNull()) return;

	UStreamableRenderAsset* LoadedMesh = CachedMeshAsset.LoadSynchronous();
	if (!LoadedMesh) return;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = InPawn;
	SpawnParams.Instigator = InPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* NewActor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity, SpawnParams);
	if (!NewActor) return;
	
	UMeshComponent* NewMeshComp = CreateMeshComponent(NewActor, LoadedMesh);
	
	if (!NewMeshComp)
	{
		NewActor->Destroy();
		return;
	}
	
	NewMeshComp->RegisterComponent();
	NewActor->SetRootComponent(NewMeshComp);
	NewActor->AttachToComponent(InAttachTarget, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocketName);

	SpawnedActors.Add(NewActor);
}

UMeshComponent* UKREquipmentInstance::CreateMeshComponent(AActor* InOwnerActor, UStreamableRenderAsset* InMeshAsset)
{
	if (UStaticMesh* SM = Cast<UStaticMesh>(InMeshAsset))
	{
		UStaticMeshComponent* SMC = NewObject<UStaticMeshComponent>(InOwnerActor);
		SMC->SetStaticMesh(SM);
		return SMC;
	}
	
	if (USkeletalMesh* SKM = Cast<USkeletalMesh>(InMeshAsset))
	{
		USkeletalMeshComponent* SKMC = NewObject<USkeletalMeshComponent>(InOwnerActor);
		SKMC->SetSkeletalMesh(SKM);
		return SKMC;
	}

	return nullptr;
}