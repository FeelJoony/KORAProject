#include "Game/KRGameInstance.h"
#include "SubSystem/KRCitizenStreamingSubsystem.h"
#include "SubSystem/KRDataTablesSubsystem.h"

void UKRGameInstance::Init()
{
    Super::Init();

    /*auto* DataTables = UKRDataTablesSubsystem::GetSafe(this);
    auto* Streaming = GetSubsystem<UKRCitizenStreamingSubsystem>();

    if (!DataTables || !Streaming) return;

    if (UCacheDataTable* Cache = DataTables->GetTable(FCitizenDataStruct::StaticStruct()))
    {
        Streaming->SetAppearanceTable(Cache->GetTable(), true);
    }*/
}
