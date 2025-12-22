#include "Equipment/KREquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentInstance)

UKREquipmentInstance::UKREquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKREquipmentInstance::InitializeFromData(const FEquipDataStruct* InData)
{
	Instigator = GetOuter();

	if (InData)
	{
		EquipDataCopy = *InData;
	}
}