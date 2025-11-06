#include "Data/CacheDataTable.h"
#include "Engine/DataTable.h"
#include "Interface/TableKey.h"
#include "Data/SampleDataStruct.h"
#include "Data/WeaponItemDataStruct.h"
#include "Data/ConsumeItemDataStruct.h"
#include "Data/MaterialItemDataStruct.h"

void UCacheDataTable::Init(EGameDataType InDataType, UDataTable* NewDataTable)
{
	switch (InDataType)
	{
		case EGameDataType::SampleData:
		{
			InitKeyList<FSampleDataStruct>(NewDataTable);
		}
		break;

		case EGameDataType::WeaponItemData:
		{
			InitKeyList<FWeaponItemDataStruct>(NewDataTable);
		}
		break;

		case EGameDataType::ConsumeItemData:
		{
			InitKeyList<FConsumeItemDataStruct>(NewDataTable);
		}
		break;

		case EGameDataType::MaterialItemData:
		{
			InitKeyList<FMaterialItemDataStruct>(NewDataTable);
		}
		break;
	}
}
