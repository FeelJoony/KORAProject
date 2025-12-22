#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Data/EquipDataStruct.h"
#include "KREquipmentDefinition.generated.h"


UCLASS(Blueprintable, BlueprintType)
class KORAPROJECT_API UKREquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UKREquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// DataTable Row 포인터 대신 복사본의 주소를 반환 (Dangling Pointer 방지)
	FORCEINLINE const FEquipDataStruct* GetEquipDataStruct() const { return &EquipDataStructCopy; }
	FORCEINLINE const FGameplayTag& GetEquipmentSlotTag() const { return EquipDataStructCopy.SlotTag; }

	// 데이터가 유효한지 확인
	FORCEINLINE bool IsValid() const { return EquipDataStructCopy.EquipItemTag.IsValid(); }

protected:
	friend class UKREquipmentInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	TObjectPtr<UObject> Instigator;

	// DataTable Row 포인터 대신 복사본 저장 (Dangling Pointer 방지)
	// DataTable이 Hot Reload 또는 GC로 언로드되어도 안전함
	UPROPERTY()
	FEquipDataStruct EquipDataStructCopy;

};