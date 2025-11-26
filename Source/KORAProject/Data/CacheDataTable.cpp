#include "Data/CacheDataTable.h"

#include "Engine/DataTable.h"
#include "Data/SampleDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "Data/WeaponDataStruct.h"
#include "Data/WeaponEnhanceDataStruct.h"

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

		case EGameDataType::WeaponEnhancedData:
		{
			InitKeyList<FWeaponEnhanceDataStruct>(NewDataTable);
		}
		break;
	}
}
