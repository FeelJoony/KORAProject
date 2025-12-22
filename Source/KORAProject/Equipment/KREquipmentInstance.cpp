#include "Equipment/KREquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentInstance)

UKREquipmentInstance::UKREquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKREquipmentInstance::InitializeFromData(const FEquipDataStruct* InData)
{
	Definition = NewObject<UKREquipmentDefinition>(this);
	Definition->Instigator = GetOuter();

	// DataTable Row 포인터 대신 복사본 저장 (Dangling Pointer 방지)
	if (InData)
	{
		Definition->EquipDataStructCopy = *InData;
	}
}