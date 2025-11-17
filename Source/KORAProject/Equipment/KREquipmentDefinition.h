// #pragma once
//
// #include "Templates/SubclassOf.h"
// #include "GameplayTagContainer.h"
// #include "KREquipmentDefinition.generated.h"
//
// class AActor;
// class UKREquipmentInstance;
// class UGameplayAbility;
//
// USTRUCT()
// struct FKREquipmentActorToSpawn
// {
// 	GENERATED_BODY()
//
// 	FKREquipmentActorToSpawn()
// 	{}
//
// 	UPROPERTY(EditAnywhere, Category=Equipment)
// 	TSubclassOf<AActor> ActorToSpawn;
//
// 	UPROPERTY(EditAnywhere, Category=Equipment)
// 	FName AttachSocket;
//
// 	UPROPERTY(EditAnywhere, Category=Equipment)
// 	FTransform AttachTransform;
// };
//
// UCLASS()
// class KORAPROJECT_API UKREquipmentDefinition : public UObject
// {
// 	GENERATED_BODY()
// 	
// public:
// 	UKREquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
// 	
// 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
// 	TSubclassOf<UKREquipmentInstance> InstanceType;
// 	
// 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
// 	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant;
//
// 	UPROPERTY(EditDefaultsOnly, Category = Equipment)
// 	TArray<FGameplayTag> EquipmentSlotTags;
// 	
// 	UPROPERTY(EditDefaultsOnly, Category=Equipment)
// 	TArray<FKREquipmentActorToSpawn> ActorsToSpawn;
// };