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
#include "KRSoundSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameplayTag/KRNPCTag.h"
#include "GameplayTag/KRSoundTag.h"

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
	
	FGameplayTag TableKey = ActivationTag.IsValid() ? ActivationTag : KRTAG_NPC_TYPE_CLARA;
	
	UCacheDataTable* CacheTable = DataTableSubsystem.GetTable(EGameDataType::LevelTransitionData);
	FLevelTransitionDataStruct* TransitionData = nullptr;

	if (CacheTable)
	{
		TransitionData = CacheTable->FindRowSafe<FLevelTransitionDataStruct>(TableKey, TEXT("MapTravel"), false);
	}

	// TransitionData가 없으면 기본값 사용
	FLevelTransitionDataStruct DefaultTransitionData;
	const FLevelTransitionDataStruct& TransitionDataRef = TransitionData ? *TransitionData : DefaultTransitionData;
	StartTransitionSequence(Experience, TransitionDataRef, ActivationTag);
}

void UKRMapTravelSubsystem::StartTransitionSequence(
    const UKRUserFacingExperience* Experience,
    const FLevelTransitionDataStruct& TransitionData,
    FGameplayTag ActivationTag)
{
    if (!Experience) { return; }

    bIsTransitioning = true;
    OnMapTravelStarted.Broadcast();
	
    PendingLevelSoundTag = TransitionData.LevelEnterSoundTag;
	PendingLevelNameKey = FName(TransitionData.StringTableKey.ToString());

    if (UKRUIRouterSubsystem* UIRouter = GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>())
    {
        UIRouter->OpenRoute(FName(TEXT("FadeOut")));
    }

	ExecuteServerTravel(Experience);

	// 우선 이 부분 때문에 원래 약간 늦게 다음 레벨로 넘어가졌고
    // GetGameInstance()->GetTimerManager().SetTimer(
    //     FadeTimerHandle,
    //     [this, Experience]()
    //     {
    //     	ExecuteServerTravel(Experience);
    //     },
    //     FadeOutDuration,
    //     false
    // );
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

void UKRMapTravelSubsystem::BeginAsyncLoadingCheck()
{
	bWaitingForAsyncLoading = true;

	GetGameInstance()->GetTimerManager().SetTimer(
		AsyncLoadingCheckTimer,
		this,
		&UKRMapTravelSubsystem::CheckAsyncLoading,
		0.1f,
		true
	);
}

void UKRMapTravelSubsystem::CheckAsyncLoading()
{
	if (IsAsyncLoading())
	{
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World) return;

	for (ULevelStreaming* StreamingLevel : World->GetStreamingLevels())
	{
		if (StreamingLevel && StreamingLevel->ShouldBeVisible() && !StreamingLevel->IsLevelVisible())
		{
			return;
		}
	}
	
	GetGameInstance()->GetTimerManager().ClearTimer(AsyncLoadingCheckTimer);
	bWaitingForAsyncLoading = false;
	
	OnAsyncLoadingFinished();
}

void UKRMapTravelSubsystem::OnAsyncLoadingFinished()
{
	bAsyncLoadingFinished = true;
	TryFinishLoading();
}

void UKRMapTravelSubsystem::OnMinLoadingTimeElapsed()
{
	bMinLoadingTimeElapsed = true;
	TryFinishLoading();
}

void UKRMapTravelSubsystem::TryFinishLoading()
{
	if (!bMinLoadingTimeElapsed || !bAsyncLoadingFinished)
	{
		return;
	}

	if (UKRUIRouterSubsystem* UIRouter =
		GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>())
	{
		UIRouter->CloseRoute(FName(TEXT("Loading")));
	}

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}

		if (ULocalPlayer* LP = World->GetFirstLocalPlayerFromController())
		{
			if (UKRUIInputSubsystem* UIInput = LP->GetSubsystem<UKRUIInputSubsystem>())
			{
				UIInput->ForceResetUIMode(); 
			}
		}
	}
	
	OnLevelLoadComplete(
		PendingLevelNameKey,
		PendingLevelSoundTag,
		PendingDisplayDuration
	);
}


void UKRMapTravelSubsystem::OnWorldInitialized(
	UWorld* World, const UWorld::InitializationValues IVS)
{
	if (!bIsTransitioning) return;
	if (World->GetGameInstance() != GetGameInstance()) return;

	bIsTransitioning = false;

	// Loading UI는 다음 Tick에서 열기 (UIRouter 안정화)
	GetGameInstance()->GetTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateLambda([this]()
		{
			if (UKRUIRouterSubsystem* UIRouter = GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>())
			{
				UIRouter->OpenRoute(FName(TEXT("Loading")));
			}

			// 최소 로딩 시간 타이머
			bMinLoadingTimeElapsed = false;
			GetGameInstance()->GetTimerManager().SetTimer(
				MinLoadingTimerHandle,
				this,
				&UKRMapTravelSubsystem::OnMinLoadingTimeElapsed,
				MinLoadingDuration,
				false
			);

			// Async / Streaming 체크
			bAsyncLoadingFinished = false;
			BeginAsyncLoadingCheck();
		})
	);
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

	//SoundTag = KRTAG_SOUND_UI_MAPTRANSITION;
	
	if (SoundTag.IsValid())
	{
		UE_LOG(LogMapTravel, Log, TEXT("Playing sound: %s"), *SoundTag.ToString());

		// if (UWorld* World = GetWorld())
		// {
		// 	if (UKRSoundSubsystem* SoundSubsystem =
		// 		World->GetSubsystem<UKRSoundSubsystem>())
		// 	{
		// 		FVector Location = FVector::ZeroVector;
		//
		// 		if (APlayerController* PC = World->GetFirstPlayerController())
		// 		{
		// 			if (APawn* Pawn = PC->GetPawn())
		// 			{
		// 				Location = Pawn->GetActorLocation();
		// 			}
		// 		}
		//
		// 		SoundSubsystem->PlaySoundByTag(
		// 			SoundTag,
		// 			Location, 
		// 			nullptr, 
		// 			true  
		// 		);
		// 	}
		// }
	}

}