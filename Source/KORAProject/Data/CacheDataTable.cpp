#include "Data/CacheDataTable.h"

#include "Engine/DataTable.h"
#include "Data/SampleDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "Data/WeaponDataStruct.h"
#include "Data/WeaponEnhanceDataStruct.h"
#include "Data/TutorialDataStruct.h"
#include "Data/ShopItemDataStruct.h"
#include "Data/EquipmentDataStruct.h"
#include "Data/ConsumeDataStruct.h"

struct FConsumeDataStruct;

void UCacheDataTable::Init(EGameDataType InDataType, UDataTable* NewDataTable)
{
	switch (InDataType)
	{
	case EGameDataType::SampleData:
		{
			InitKeyList<FSampleDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::ItemData:
		{
			InitKeyList<FItemDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::WeaponData:
		{
			InitKeyList<FWeaponDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::WeaponEnhanceData:
		{
			InitKeyList<FWeaponEnhanceDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::TutorialData:
		{
			InitKeyList<FWeaponEnhanceDataStruct>(NewDataTable);
		}
		break;
	
	case EGameDataType::ShopItemData:
		{
			InitKeyList<FShopItemDataStruct>(NewDataTable);
		}
		break;

	case EGameDataType::EquipmentData:
		{
			InitKeyList<FEquipmentDataStruct>(NewDataTable);
		}
		break;
	

	case EGameDataType::ConsumeData:
		{
			InitKeyList<FConsumeDataStruct>(NewDataTable);
		}
	break;
	}
}

