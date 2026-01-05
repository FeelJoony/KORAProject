#include "Game/KRGameInstance.h"
#include "SubSystem/KRCitizenStreamingSubsystem.h"
#include "SubSystem/KRDataTablesSubsystem.h"

void UKRGameInstance::Init()
{
    Super::Init();

    /*auto* DataTables = GetSubsystem<UKRDataTablesSubsystem>();
    auto* Streaming = GetSubsystem<UKRCitizenStreamingSubsystem>();

    if (!DataTables || !Streaming) return;

    if (UCacheDataTable* Cache =
        DataTables->GetTable(EGameDataType::CitizenData))
    {
        Streaming->SetAppearanceTable(Cache->GetTable(), true);
    }*/
}
