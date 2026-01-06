#include "SubSystem/KRMapTravelSubsystem.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "SubSystem/KRQuestSubsystem.h"
#include "Data/LevelTransitionDataStruct.h"
#include "Data/GameDataType.h"
#include "Data/CacheDataTable.h"
#include "GameModes/KRUserFacingExperience.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameplayTag/KRNPCTag.h"

DEFINE_LOG_CATEGORY_STATIC(LogMapTravel, Log, All);

void UKRMapTravelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UKRDataTablesSubsystem>();
	Collection.InitializeDependency<UKRUIRouterSubsystem>();
	
	WorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UKRMapTravelSubsystem::OnWorldInitialized);

	bIsTransitioning = false;
}

void UKRMapTravelSubsystem::Deinitialize()
{
	if (WorldInitHandle.IsValid())
	{
		FWorldDelegates::OnPostWorldInitialization.Remove(WorldInitHandle);
		WorldInitHandle.Reset();
	}

	Super::Deinitialize();
}

UKRMapTravelSubsystem& UKRMapTravelSubsystem::Get(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	check(World);
	UKRMapTravelSubsystem* MapTravelSubsystem = UGameInstance::GetSubsystem<UKRMapTravelSubsystem>(World->GetGameInstance());
	check(MapTravelSubsystem);
	return *MapTravelSubsystem;
}

void UKRMapTravelSubsystem::TravelToExperience(const FString& UserFacingPath, FGameplayTag ActivationTag)
{
	UKRUserFacingExperience* Experience = LoadObject<UKRUserFacingExperience>(nullptr, *UserFacingPath);
	if (!Experience)
	{
		UE_LOG(LogMapTravel, Error, TEXT("Failed to load UserFacingExperience: %s"), *UserFacingPath);
		return;
	}
	
	UKRDataTablesSubsystem& DataTableSubsystem = UKRDataTablesSubsystem::Get(this);
	
	FName TableKey = NAME_None;

	if (!ActivationTag.IsValid())
	{
		TableKey = FName(TEXT("NPC.Type.Clara"));
	}
	else
	{
		TableKey = ActivationTag.GetTagName();
	}
	
	UCacheDataTable* CacheTable = DataTableSubsystem.GetTable(EGameDataType::LevelTransitionData);
	FLevelTransitionDataStruct* TransitionData = nullptr;

	if (CacheTable && CacheTable->GetTable())
	{
		TransitionData = CacheTable->GetTable()->FindRow<FLevelTransitionDataStruct>(TableKey, TEXT("MapTravel"), false);
	}

	// TransitionData가 없으면 기본값 사용
	FLevelTransitionDataStruct DefaultTransitionData;
	const FLevelTransitionDataStruct& TransitionDataRef = TransitionData ? *TransitionData : DefaultTransitionData;

	if (!TransitionData)
	{
		UE_LOG(LogMapTravel, Warning, TEXT("No transition data found for key: %s. Using default values."), *TableKey.ToString());
	}

	StartTransitionSequence(Experience, TransitionDataRef, ActivationTag);
}

void UKRMapTravelSubsystem::StartTransitionSequence(
    const UKRUserFacingExperience* Experience,
    const FLevelTransitionDataStruct& TransitionData,
    FGameplayTag ActivationTag)
{
    if (!Experience)
    {
        return;
    }

    bIsTransitioning = true;
    OnMapTravelStarted.Broadcast();
	
    PendingLevelSoundTag = TransitionData.LevelEnterSoundTag;

    if (UKRUIRouterSubsystem* UIRouter = GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>())
    {
        UIRouter->OpenRoute(FName(TEXT("Loading")));
    }
	
	PendingLevelNameKey = FName(TransitionData.StringTableKey.ToString());
	
	
    FTimerHandle FadeTimerHandle;
    GetGameInstance()->GetTimerManager().SetTimer(
        FadeTimerHandle,
        [this, Experience]()
        {
            ExecuteServerTravel(Experience);
        },
        FadeOutDuration,
        false
    );
}

void UKRMapTravelSubsystem::ExecuteServerTravel(const UKRUserFacingExperience* Experience)
{
	if (!Experience)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* HostingPlayer = World->GetFirstPlayerController();
	if (!HostingPlayer)
	{
		UE_LOG(LogMapTravel, Error, TEXT("No valid PlayerController found!"));
		return;
	}

	if (UCommonSessionSubsystem* SessionSubsystem = GetGameInstance()->GetSubsystem<UCommonSessionSubsystem>())
	{
		UCommonSession_HostSessionRequest* Request = Experience->CreateHostingRequest();
		if (Request)
		{
			UE_LOG(LogMapTravel, Log, TEXT("Hosting session with MapID: %s"),
				*Experience->MapID.ToString());
			SessionSubsystem->HostSession(HostingPlayer, Request);
		}
	}
}

void UKRMapTravelSubsystem::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (!bIsTransitioning)
	{
		UE_LOG(LogMapTravel, Warning, TEXT("[OnWorldInitialized] Ignored - Not transitioning"));
		return;
	}
	
	if (World && World->GetGameInstance() == GetGameInstance())
	{
		bIsTransitioning = false;
		
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (ULocalPlayer* LP = PC->GetLocalPlayer())
			{
				if (UKRUIInputSubsystem* UIInputSubsystem = LP->GetSubsystem<UKRUIInputSubsystem>())
				{
					UIInputSubsystem->ForceResetUIMode();
				}
			}
		}
		
		FTimerHandle LoadingTimerHandle;
		GetGameInstance()->GetTimerManager().SetTimer(
			LoadingTimerHandle,
			[this]()
			{
				if (UKRUIRouterSubsystem* UIRouter = GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>())
				{
					UIRouter->CloseRoute(FName(TEXT("Loading")));
				}
				
				OnLevelLoadComplete(PendingLevelNameKey, PendingLevelSoundTag, PendingDisplayDuration);
			},
			2.5f,
			false
		);
	}
}

void UKRMapTravelSubsystem::OnLevelLoadComplete(FName StringTableKey, FGameplayTag SoundTag, float DisplayDuration)
{
	OnMapTravelCompleted.Broadcast();
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

	FKRUIMessage_Info Message;
	Message.Context = EInfoContext::LevelTransition;
	Message.StringTableKey = StringTableKey.ToString();

	MessageSubsystem.BroadcastMessage(
		FKRUIMessageTags::SaveDeathLevelInfo(),
		Message
	);
	
	if (SoundTag.IsValid())
	{
		UE_LOG(LogMapTravel, Log, TEXT("Playing sound: %s"), *SoundTag.ToString());
		
		// if (UKRSoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<UKRSoundSubsystem>())
		// {
		//     SoundSubsystem->PlaySound(SoundTag);
		// }
	}
	
}