#include "Data/CacheDataTable.h"

#include "QuestDataStruct.h"
#include "SubQuestDataStruct.h"
#include "TutorialDataStruct.h"

#include "EquipDataStruct.h"
#include "ModuleDataStruct.h"
#include "EquipAbilityDataStruct.h"
#include "WeaponEnhanceDataStruct.h"

#include "ItemDataStruct.h"
#include "ShopItemDataStruct.h"
#include "ConsumeDataStruct.h"
#include "CurrencyDataStruct.h"

struct FConsumeDataStruct;

void UCacheDataTable::Init(EGameDataType InDataType, UDataTable* NewDataTable)
{
	switch (InDataType)
	{
	case EGameDataType::ItemData:
		{
			InitKeyList<FItemDataStruct>(NewDataTable);
		}
		break;
		
	case EGameDataType::WeaponEnhanceData:
		{
			InitKeyList<FWeaponEnhanceDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::TutorialData:
		{
			InitKeyList<FTutorialDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::ShopItemData:
		{
			InitKeyList<FShopItemDataStruct>(NewDataTable);
		}
		break;
	
	case EGameDataType::ConsumeData:
		{
			InitKeyList<FConsumeDataStruct>(NewDataTable);
		}
		break;
	
	case EGameDataType::QuestData:
		{
			InitKeyList<FQuestDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::SubQuestData:
		{
			InitKeyList<FSubQuestDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::EquipData:
		{
			InitKeyList<FEquipDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::EquipAbilityData:
		{
			InitKeyList<FEquipAbilityDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::ModuleData:
		{
			InitKeyList<FModuleDataStruct>(NewDataTable);
		}
		break;
		
	case EGameDataType::CurrencyData:
		{
			InitKeyList<FCurrencyDataStruct>(NewDataTable);
		}
		break;
	}
}