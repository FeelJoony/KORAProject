#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "KREquipmentDefinition.generated.h"

class AActor;
class UKREquipmentInstance;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FKREquipmentActorToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment)
	FTransform AttachTransform;
};

/**
 * Equipment Definition
 * 장착 시 생성될 Instance, 부여할 Ability, 스폰할 Actor 정의
 */
UCLASS(Blueprintable, BlueprintType)
class KORAPROJECT_API UKREquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UKREquipmentDefinition(const FObjectInitializer& ObjectInitializer);
	
	// Equipment Instance 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment)
	TSubclassOf<UKREquipmentInstance> InstanceType;

	// 부여할 Abilities
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment)
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant;

	// 장착 슬롯 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment)
	FGameplayTag EquipmentSlotTag; // ex) Equipment.Slot.MainWeapon

	// 스폰할 Actors
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment)
	TArray<FKREquipmentActorToSpawn> ActorsToSpawn;
};