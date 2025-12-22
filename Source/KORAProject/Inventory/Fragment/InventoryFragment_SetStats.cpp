#include "InventoryFragment_SetStats.h"

#include "Inventory/KRInventoryItemInstance.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Data/EquipAbilityDataStruct.h"
#include "Data/EquipDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "GameplayTag/KRItemTypeTag.h"

void UInventoryFragment_SetStats::OnInstanceCreated(UKRInventoryItemInstance* Instance)
{
	if (!Instance) return;

	const FGameplayTag EquipTag = Instance->GetItemTag();
	if (!EquipTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORD) && !EquipTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUN))
	{
		return;
	}

	InitializeWeaponStats(Instance);
}

void UInventoryFragment_SetStats::InitializeWeaponStats(UKRInventoryItemInstance* Instance)
{
	if (!Instance) return;
	UObject* ContextObj = Instance->GetOwnerContext();
	if (!ContextObj) return;
	
	UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(ContextObj);

	const FItemDataStruct* ItemData = DataTablesSubsystem.GetData<FItemDataStruct>(EGameDataType::ItemData, Instance->GetItemTag());
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
	if (!AbilityData)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipAbilityData Missing"));
		return;
	}
	
	CachedStats.Reset();
	auto ApplyModifier = [](float& StatValue, const FKREquipAbilityModifierRow& Row)
	{
		const float Value = Row.GetFinalValue();
		switch (Row.Op)
		{
		case EModifierOp::Absolute:
			StatValue = Value;
			break;
		case EModifierOp::AdditiveDelta:
			StatValue += Value;
			break;
		case EModifierOp::Multiplier:
			StatValue *= Value;
			break;
		}
	};
	
	for (const FKREquipAbilityModifierRow& Row : AbilityData->AbilityModifiers)
	{
		if (!Row.AbilityTypeTag.IsValid())
		{
			continue;
		}

		if (Row.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_ATK))
		{
			ApplyModifier(CachedStats.AttackPower, Row);
		}
		else if (Row.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_ATTACKSPEED))
		{
			ApplyModifier(CachedStats.AttackSpeed, Row);
		}
		else if (Row.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_RANGE))
		{
			ApplyModifier(CachedStats.Range, Row);
		}
		else if (Row.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_CRITCHANCE))
		{
			ApplyModifier(CachedStats.CritChance, Row);
		}
		else if (Row.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_CRITMULTI))
		{
			ApplyModifier(CachedStats.CritMulti, Row);
		}
		else if (Row.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_CAPACITY))
		{
			const float Value = Row.GetFinalValue();
			switch (Row.Op)
			{
			case EModifierOp::Absolute:
				CachedStats.Capacity = static_cast<int32>(Value);
				break;
			case EModifierOp::AdditiveDelta:
				CachedStats.Capacity += static_cast<int32>(Value);
				break;
			case EModifierOp::Multiplier:
				CachedStats.Capacity = static_cast<int32>(CachedStats.Capacity * Value);
				break;
			}
		}
		else if (Row.AbilityTypeTag.MatchesTag(KRTAG_WEAPON_STAT_RELOADTIME))
		{
			ApplyModifier(CachedStats.ReloadTime, Row);
		}
	}

	CachedStats.MarkInitialized();
	bIsInitialized = true;
}

