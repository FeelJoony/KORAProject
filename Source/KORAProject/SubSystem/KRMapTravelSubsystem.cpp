#include "SubSystem/KRMapTravelSubsystem.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "SubSystem/KRLoadingSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "Data/LevelTransitionDataStruct.h"
#include "Data/GameDataType.h"
#include "Data/CacheDataTable.h"
#include "GameModes/KRUserFacingExperience.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "KRQuestSubsystem.h"
#include "Engine/LocalPlayer.h"
#include "GameplayTag/KRNPCTag.h"
#include "Quest/KRQuestInstance.h"

DEFINE_LOG_CATEGORY_STATIC(LogMapTravel, Log, All);

void UKRMapTravelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UKRDataTablesSubsystem>();

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

FGameplayTag UKRMapTravelSubsystem::ResolveObjectiveTag() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return FGameplayTag();
	}

	UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(World);

	if (UKRQuestInstance* ActiveQuest = QuestSubsystem.GetActiveInstance())
	{
		const FGameplayTag QuestObjectiveTag =
			ActiveQuest->GetSubQuestEvalDataStruct().ObjectiveTag;

		if (QuestObjectiveTag.IsValid())
		{
			return QuestObjectiveTag;
		}
	}

	return KRTAG_NPC_TYPE_CLARA;
}


void UKRMapTravelSubsystem::TravelToExperience(const FString& UserFacingPath)
{
	UKRUserFacingExperience* Experience = LoadObject<UKRUserFacingExperience>(nullptr, *UserFacingPath);
	if (!Experience)
	{
		UE_LOG(LogMapTravel, Error, TEXT("Failed to load UserFacingExperience: %s"), *UserFacingPath);
		return;
	}

	const FGameplayTag ObjectiveTag = ResolveObjectiveTag();
	
	UKRDataTablesSubsystem& DataTableSubsystem = UKRDataTablesSubsystem::Get(this);

	// GetTable로 직접 DataTable 접근
	UCacheDataTable* CacheTable = DataTableSubsystem.GetTable(FLevelTransitionDataStruct::StaticStruct());
	FLevelTransitionDataStruct* TransitionData = nullptr;

	if (CacheTable)
	{
		TransitionData = CacheTable->FindRowSafe<FLevelTransitionDataStruct>(ObjectiveTag, TEXT("MapTravel"), false);
	}
	
	FLevelTransitionDataStruct DefaultTransitionData;
	const FLevelTransitionDataStruct& TransitionDataRef = TransitionData ? *TransitionData : DefaultTransitionData;
	
	StartTransitionSequence(Experience, TransitionDataRef, ObjectiveTag);
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

    UKRLoadingSubsystem::Get().ShowLoadingScreen();

	ExecuteServerTravel(Experience);
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

	bMinLoadingTimeElapsed = false;
	GetGameInstance()->GetTimerManager().SetTimer(
		MinLoadingTimerHandle,
		this,
		&UKRMapTravelSubsystem::OnMinLoadingTimeElapsed,
		MinLoadingDuration,
		false
	);

	bAsyncLoadingFinished = false;
	BeginAsyncLoadingCheck();
}

void UKRMapTravelSubsystem::OnLevelLoadComplete(FName StringTableKey, FGameplayTag SoundTag, float DisplayDuration)
{
	OnMapTravelCompleted.Broadcast();
	
	LoadingScreenHiddenHandle = UKRLoadingSubsystem::Get().OnLoadingScreenHidden.AddUObject(
		this, &UKRMapTravelSubsystem::OnLoadingScreenHiddenCallback);

	UKRLoadingSubsystem::Get().HideLoadingScreen();
}

void UKRMapTravelSubsystem::OnLoadingScreenHiddenCallback()
{
	UKRLoadingSubsystem::Get().OnLoadingScreenHidden.Remove(LoadingScreenHiddenHandle);
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

	FKRUIMessage_Info Message;
	Message.Context = EInfoContext::LevelTransition;
	Message.StringTableKey = PendingLevelNameKey.ToString();

	MessageSubsystem.BroadcastMessage(
		FKRUIMessageTags::SaveDeathLevelInfo(),
		Message
	);

	// Restore Quest UI after level transition
	UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(GetWorld());
	QuestSubsystem.RespawnQuestActorForLevelTransition();
	if (UKRQuestInstance* ActiveQuest = QuestSubsystem.GetActiveInstance())
	{
		if (ActiveQuest->GetQuestState() == EQuestState::InProgress)
		{
			const FSubQuestEvalDataStruct& CurrentEvalData = ActiveQuest->GetSubQuestEvalDataStruct();

			if (CurrentEvalData.UIRowName.IsValid())
			{
				FKRUIMessage_Quest QuestMessage;
				QuestMessage.QuestNameKey = CurrentEvalData.UIRowName;
				MessageSubsystem.BroadcastMessage(FKRUIMessageTags::Quest(), QuestMessage);
			}
		}
	}
}