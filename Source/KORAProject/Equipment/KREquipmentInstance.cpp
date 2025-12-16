#include "Equipment/KREquipmentInstance.h"
#include "Equipment/KREquipmentDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "KREquipmentManagerComponent.h"
#include "Data/EquipmentDataStruct.h"
#include "Data/EquipDataStruct.h"
#include "Components/MeshComponent.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Item/Weapons/KRWeaponBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KREquipmentInstance)

UKREquipmentInstance::UKREquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKREquipmentInstance::InitializeFromData(const FEquipDataStruct* InData)
{
	Definition = NewObject<UKREquipmentDefinition>(this);
	Definition->Instigator = GetOuter();
	Definition->EquipDataStruct = InData;
}