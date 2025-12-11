#include "Game/KRGameInstance.h"
#include "SubSystem/KRCitizenStreamingSubsystem.h"

void UKRGameInstance::Init()
{
    Super::Init();

    if (UKRCitizenStreamingSubsystem* Streaming = GetSubsystem<UKRCitizenStreamingSubsystem>())
    {
        Streaming->PreloadAppearances();
    }
}
