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

void UKRGameInstance::SaveCheckpoint(const FTransform& SpawnTransform, FName CheckpointTag)
{
    LastCheckpointTransform = SpawnTransform;
    LastCheckpointTag = CheckpointTag;
    bHasCheckpoint = true;

    UE_LOG(LogTemp, Log, TEXT("[GameInstance] Checkpoint saved at %s"), *SpawnTransform.GetLocation().ToString());
}

void UKRGameInstance::ClearCheckpoint()
{
    LastCheckpointTransform = FTransform::Identity;
    LastCheckpointTag = NAME_None;
    bHasCheckpoint = false;

    UE_LOG(LogTemp, Log, TEXT("[GameInstance] Checkpoint cleared"));
}
