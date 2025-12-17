#include "InventoryFragment_SetStats.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/EquipAbilityDataStruct.h"
#include "Data/EquipDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "GAS/AttributeSets/KRWeaponAttributeSet.h"

static FGameplayAttribute ConvertTagToWeaponAttribute(const FGameplayTag& Tag)
{
	if (Tag.MatchesTag(KRTAG_WEAPON_STAT_ATK)) return UKRWeaponAttributeSet::GetAttackPowerAttribute();
	if (Tag.MatchesTag(KRTAG_WEAPON_STAT_ATTACKSPEED)) return UKRWeaponAttributeSet::GetAttackSpeedAttribute();
	if (Tag.MatchesTag(KRTAG_WEAPON_STAT_RANGE)) return UKRWeaponAttributeSet::GetRangeAttribute();
	if (Tag.MatchesTag(KRTAG_WEAPON_STAT_CRITCHACNE)) return UKRWeaponAttributeSet::GetCritChanceAttribute();
	if (Tag.MatchesTag(KRTAG_WEAPON_STAT_CRITMULTI)) return UKRWeaponAttributeSet::GetCritMultiAttribute();
	if (Tag.MatchesTag(KRTAG_WEAPON_STAT_CAPACITY)) return UKRWeaponAttributeSet::GetCapacityAttribute();
	if (Tag.MatchesTag(KRTAG_WEAPON_STAT_RELOADTIME)) return UKRWeaponAttributeSet::GetReloadTimeAttribute();
	
	return FGameplayAttribute();
}


void UInventoryFragment_SetStats::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	if (Instance)
	{
		InitializeWeaponStats(Instance->GetItemTag());
	}
}

void UInventoryFragment_SetStats::InitializeWeaponStats(const FGameplayTag& ItemTag)
{
	if (!WeaponAttributeSet)
	{
		WeaponAttributeSet = NewObject<UKRWeaponAttributeSet>(this);
	}

	UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(this);

	const FItemDataStruct* ItemData = DataTablesSubsystem.GetData<FItemDataStruct>(EGameDataType::ItemData, ItemTag);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemData Missing"));
		return;
	}

	const FEquipDataStruct* EquipData = DataTablesSubsystem.GetData<FEquipDataStruct>(EGameDataType::EquipData, ItemData->EquipID);
	if (!EquipData)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipData Missing"));
		return;
	}

	const FEquipAbilityDataStruct* AbilityData = DataTablesSubsystem.GetData<FEquipAbilityDataStruct>(EGameDataType::EquipAbilityData, EquipData->EquipAbilityID);

	if (AbilityData)
	{
		for (const FKREquipAbilityModifierRow& Row : AbilityData->AbilityModifiers)
		{
			FGameplayAttribute Attribute = ConvertTagToWeaponAttribute(Row.AbilityTypeTag);

			if (Attribute.IsValid())
			{
				float FinalValue = Row.BaseValue + Row.IncValue;

				Attribute.SetNumericValueChecked(FinalValue, WeaponAttributeSet);
			}
		}
	}
}

