// #include "Equipment/KREquipmentInstance.h"
// #include "Equipment/KREquipmentDefinition.h"
// #include "Components/SkeletalMeshComponent.h"
// #include "GameFramework/Character.h"
// #include "Inventory/KRInventoryItemInstance.h"
// #include "Inventory/KRInventoryItemDefinition.h"
// #include "Inventory/InventoryFragment_EquippableItem.h"
// #include "GameplayTagContainer.h"
//
// #include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentInstance)
//
// UKREquipmentInstance::UKREquipmentInstance(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// }
//
// UWorld* UKREquipmentInstance::GetWorld() const
// {
// 	if (APawn* OwningPawn = GetPawn())
// 	{
// 		return OwningPawn->GetWorld();
// 	}
// 	else
// 	{
// 		return nullptr;
// 	}
// }
//
// APawn* UKREquipmentInstance::GetPawn() const
// {
// 	return Cast<APawn>(GetOuter());
// }
//
// APawn* UKREquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
// {
// 	APawn* Result = nullptr;
// 	if (UClass* ActualPawnType = PawnType)
// 	{
// 		if (GetOuter()->IsA(ActualPawnType))
// 		{
// 			Result = Cast<APawn>(GetOuter());
// 		}
// 	}
// 	return Result;
// }
//
// void UKREquipmentInstance::SpawnEquipmentActors(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
// {
// 	if (APawn* OwningPawn = GetPawn())
// 	{
// 		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
// 		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
// 		{
// 			AttachTarget = Char->GetMesh();
// 		}
//
// 		for (const FKREquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
// 		{
// 			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
// 			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
// 			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
// 			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);
//
// 			SpawnedActors.Add(NewActor);
// 		}
// 	}
// }
//
// void UKREquipmentInstance::DestroyEquipmentActors()
// {
// 	for (AActor* Actor : SpawnedActors)
// 	{
// 		if (Actor)
// 		{
// 			Actor->Destroy();
// 		}
// 	}
// 	SpawnedActors.Empty();
// }
//
// void UKREquipmentInstance::OnEquipped(const TArray<FKREquipmentActorToSpawn>& ActorsToSpawn)
// {
// 	SpawnEquipmentActors(ActorsToSpawn);
// }
//
// void UKREquipmentInstance::OnUnequipped()
// {
// 	DestroyEquipmentActors();
// }
