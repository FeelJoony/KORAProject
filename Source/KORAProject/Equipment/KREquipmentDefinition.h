#pragma once

#include "Templates/SubclassOf.h"
#include "GameplayTagContainer.h"
#include "KREquipmentDefinition.generated.h"

class AActor;
//class UKRAbilitySet;
class UKREquipmentInstance;

USTRUCT()
struct FKREquipmentActorToSpawn
{
	GENERATED_BODY()

	FKREquipmentActorToSpawn()
	{}

	UPROPERTY(EditAnywhere, Category=Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category=Equipment)
	FTransform AttachTransform;
};

UCLASS()
class KORAPROJECT_API UKREquipmentDefinition : public UObject
{
	GENERATED_BODY()
	
public:
	UKREquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TSubclassOf<UKREquipmentInstance> InstanceType;
	
	//UPROPERTY(EditDefaultsOnly, Category=Equipment)
	//TArray<TObjectPtr<const UKRAbilitySet>> AbilitySetsToGrant;
	//태그 부분으로 처리하는 곳

	//UPROPERTY(EditDefaultsOnly, Category = Equipment)
	//FGameplayTag EquipmentSlotTag;
	// 장비가 장착될 수 있는 단일 슬록 방식 

	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<FGameplayTag> EquipmentSlotTags;
	// 장비가 장착될 수 있는 모든 슬롯 방식 
	
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TArray<FKREquipmentActorToSpawn> ActorsToSpawn;
};