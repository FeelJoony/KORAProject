#include "Inventory/Fragment/InventoryFragment_ModuleItem.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/ModuleDataStruct.h"
#include "Data/EquipAbilityDataStruct.h"
#include "GameplayTag/KRItemTypeTag.h"

void UInventoryFragment_ModuleItem::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	if (Instance)
	{
		InitializeFromData(Instance);
	}
}

void UInventoryFragment_ModuleItem::InitializeFromData(UKRInventoryItemInstance* Instance)
{
	if (!Instance) return;
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj) return;

	const FGameplayTag& ItemTag = Instance->GetItemTag();
	
	UKRDataTablesSubsystem& DataSubsystem = UKRDataTablesSubsystem::Get(ContextObj);
	const FModuleDataStruct* ModuleData = DataSubsystem.GetData<FModuleDataStruct>(ItemTag);

	if (!ModuleData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ModuleFragment] ModuleData not found for [%s]"), *ItemTag.ToString());
		return;
	}
	
	if (ModuleData->SlotTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORDMODULE))
	{
		TargetWeaponSlotTag = KRTAG_ITEMTYPE_EQUIP_SWORD;
	}
	else if (ModuleData->SlotTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUNMODULE))
	{
		TargetWeaponSlotTag = KRTAG_ITEMTYPE_EQUIP_GUN;
	}
	
	const FEquipAbilityDataStruct* AbilityData = DataSubsystem.GetData<FEquipAbilityDataStruct>((ModuleData->EquipAbilityID), TEXT("ModuleFragment"), false);

	if (AbilityData)
	{
		CachedModifiers = AbilityData->AbilityModifiers;
		bIsInitialized = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ModuleFragment] EquipAbilityData not found for ID [%d]"), ModuleData->EquipAbilityID);
	}
}
