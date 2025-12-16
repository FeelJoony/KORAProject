#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Data/EquipDataStruct.h"
#include "KREquipmentDefinition.generated.h"


UCLASS(Blueprintable, BlueprintType, Abstract)
class KORAPROJECT_API UKREquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UKREquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	FORCEINLINE const struct FEquipDataStruct* GetEquipDataStruct() const { return EquipDataStruct; }
	FORCEINLINE const FGameplayTag& GetEquipmentSlotTag() const { return EquipDataStruct->SlotTag; }
	
protected:
	friend class UKREquipmentInstance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UObject> Instigator;

	const struct FEquipDataStruct* EquipDataStruct;
	
};