#include "Data/CacheDataTable.h"
#include "Engine/DataTable.h"
#include "Interface/TableKey.h"
#include "Data/SampleDataStruct.h"

void UCacheDataTable::Init(EGameDataType InDataType, UDataTable* NewDataTable)
{
	switch (InDataType)
	{
		case EGameDataType::SampleData:
		{
			InitKeyList<FSampleDataStruct>(NewDataTable);
		}
		break;
	}
}
