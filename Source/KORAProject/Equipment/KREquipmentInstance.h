#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Data/EquipDataStruct.h"
#include "KREquipmentInstance.generated.h"

class UInventoryFragment_SetStats;

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
	
	virtual void InitializeFromData(const FEquipDataStruct* InData);
	virtual void InitializeStats(const UInventoryFragment_SetStats* Stats) {}
	
	UFUNCTION(BlueprintPure, Category = Equipment)
	const FEquipDataStruct& GetEquipData() const { return EquipDataCopy; }
	UFUNCTION(BlueprintPure, Category = Equipment)
	FGameplayTag GetEquipItemTag() const { return EquipDataCopy.EquipItemTag; }
	UFUNCTION(BlueprintPure, Category = Equipment)
	FGameplayTag GetSlotTag() const { return EquipDataCopy.SlotTag; }

	UFUNCTION(BlueprintPure, Category = Equipment)
	bool IsValid() const { return EquipDataCopy.EquipItemTag.IsValid(); }
	
	bool operator==(const UKREquipmentInstance& Other) const { return EquipDataCopy.EquipItemTag == Other.EquipDataCopy.EquipItemTag; }
	bool operator==(const FGameplayTag& Other) const { return EquipDataCopy.EquipItemTag == Other; }

protected:
	UPROPERTY()
	TObjectPtr<UObject> Instigator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data") FEquipDataStruct EquipDataCopy;
};