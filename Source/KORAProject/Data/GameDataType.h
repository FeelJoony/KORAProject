#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGameDataType : uint8
{
	ItemData,
	WeaponEnhanceData,
	TutorialData,
	ShopItemData,
	ConsumeData,
	QuestData,
	SubQuestData,
	EquipData,
	EquipAbilityData,
	ModuleData,
	CurrencyData,
	SoundDefinitionData,
	EffectDefinitionData,
	WorldEventData,
	CitizenData,
};