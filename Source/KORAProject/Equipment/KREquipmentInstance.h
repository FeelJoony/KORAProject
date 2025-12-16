#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "KREquipmentDefinition.h"
#include "Data/EquipDataStruct.h"
#include "KREquipmentInstance.generated.h"

class UInventoryFragment_SetStats;
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
	
	// Instigator (장착한 주체)
	UFUNCTION(BlueprintPure, Category = Equipment)
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }
	
	virtual void InitializeFromData(const struct FEquipDataStruct* InData);
	virtual void InitializeStats(const UInventoryFragment_SetStats* Stats) {}

	bool operator==(const UKREquipmentInstance& Other) const { return Definition->GetEquipDataStruct()->EquipItemTag == Other.Definition->GetEquipDataStruct()->EquipItemTag; }
	bool operator==(const FGameplayTag& Other) const { return Definition->GetEquipDataStruct()->EquipItemTag == Other; }

	FORCEINLINE const UKREquipmentDefinition* GetDefinition() const { return Definition; }

protected:
	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Definition")
	TObjectPtr<class UKREquipmentDefinition> Definition;
};