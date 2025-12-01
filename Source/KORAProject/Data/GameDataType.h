#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EGameDataType : uint8
{
	SampleData,
	ItemData,
	EquipData,
	WeaponData,
	WeaponEnhanceData,
	TutorialData,
	ShopItemData,
	QuestData,
	SubQuestData
};