#include "SubSystem/KRMapTravelSubsystem.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "Data/LevelTransitionDataStruct.h"
#include "Data/GameDataType.h"
#include "Data/CacheDataTable.h"
#include "GameModes/KRUserFacingExperience.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "CommonSessionSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY_STATIC(LogMapTravel, Log, All);

void UKRMapTravelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UKRDataTablesSubsystem>();
	Collection.InitializeDependency<UKRUIRouterSubsystem>();

	// 레벨 로드 완료 감지를 위한 델리게이트 등록
	WorldInitHandle = FWorldDelegates::OnPostWorldInitialization.AddUObject(
		this, &UKRMapTravelSubsystem::OnWorldInitialized);

	bIsTransitioning = false;

	UE_LOG(LogMapTravel, Log, TEXT("MapTravelSubsystem Initialized"));
}

void UKRMapTravelSubsystem::Deinitialize()
{
	// 델리게이트 해제
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
	UE_LOG(LogMapTravel, Log, TEXT("TravelToExperience: Path=%s, Tag=%s"),
		*UserFacingPath, *ActivationTag.ToString());

	// 1. UserFacing 로드
	UKRUserFacingExperience* Experience = LoadObject<UKRUserFacingExperience>(nullptr, *UserFacingPath);
	if (!Experience)
	{
		UE_LOG(LogMapTravel, Error, TEXT("Failed to load UserFacingExperience: %s"), *UserFacingPath);
		return;
	}

	// 2. ObjectiveTag로 Transition Data 로드
	UKRDataTablesSubsystem& DataTableSubsystem = UKRDataTablesSubsystem::Get(this);

	// ObjectiveTag를 RowName으로 변환
	FName RowName = FName(*ActivationTag.ToString());

	// GetTable로 직접 DataTable 접근
	UCacheDataTable* CacheTable = DataTableSubsystem.GetTable(FLevelTransitionData::StaticStruct());
	FLevelTransitionData* TransitionData = nullptr;

	if (CacheTable && CacheTable->GetTable())
	{
		TransitionData = CacheTable->GetTable()->FindRow<FLevelTransitionData>(RowName, TEXT("MapTravel"), false);
	}

	if (!TransitionData)
	{
		UE_LOG(LogMapTravel, Warning, TEXT("No transition data for tag: %s, using defaults"), *RowName.ToString());
		// 기본값 사용
		FLevelTransitionData DefaultData;
		StartTransitionSequence(Experience, DefaultData, ActivationTag);
	}
	else
	{
		StartTransitionSequence(Experience, *TransitionData, ActivationTag);
	}
}

void UKRMapTravelSubsystem::StartTransitionSequence(
	const UKRUserFacingExperience* Experience,
	const FLevelTransitionData& TransitionData,
	FGameplayTag ActivationTag)
{
	if (!Experience)
	{
		return;
	}

	UE_LOG(LogMapTravel, Log, TEXT("Starting transition sequence..."));

	// 델리게이트 브로드캐스트
	OnMapTravelStarted.Broadcast();

	// 전환 중 플래그 설정
	bIsTransitioning = true;

	// 전환 데이터 저장 (레벨 로드 후 사용)
	// TransitionData에 레벨 이름이 없으면 하드코딩된 값 사용
	if (TransitionData.LevelNameStringTableKey.IsNone())
	{
		PendingLevelNameKey = GetLevelNameKeyFromTag(ActivationTag);
		UE_LOG(LogMapTravel, Warning, TEXT("Using hardcoded level name key: %s"), *PendingLevelNameKey.ToString());
	}
	else
	{
		PendingLevelNameKey = TransitionData.LevelNameStringTableKey;
	}

	PendingLevelSoundTag = TransitionData.LevelEnterSoundTag;
	PendingDisplayDuration = 3.0f;  // 레벨 이름 표시 시간

	// Loading 화면 표시
	if (UKRUIRouterSubsystem* UIRouter = GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>())
	{
		UIRouter->OpenRoute(FName(TEXT("Loading")));
	}

	// 페이드 아웃 대기 후 레벨 전환
	FTimerHandle FadeTimerHandle;
	GetGameInstance()->GetTimerManager().SetTimer(
		FadeTimerHandle,
		[this, Experience]()
		{
			ExecuteServerTravel(Experience);
		},
		TransitionData.FadeOutDuration,
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
	UE_LOG(LogMapTravel, Warning, TEXT("[OnWorldInitialized] Called - bIsTransitioning: %d"), bIsTransitioning);

	// 전환 중이 아니면 무시
	if (!bIsTransitioning)
	{
		UE_LOG(LogMapTravel, Warning, TEXT("[OnWorldInitialized] Ignored - Not transitioning"));
		return;
	}

	// 현재 게임 인스턴스의 월드인지 확인
	if (World && World->GetGameInstance() == GetGameInstance())
	{
		UE_LOG(LogMapTravel, Warning, TEXT("New world initialized, showing loading screen for 2-3 seconds"));

		// 전환 중 플래그 해제
		bIsTransitioning = false;

		// UIInputSubsystem의 RefCount를 강제로 리셋 (레벨 전환 전 UI 상태 초기화)
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (ULocalPlayer* LP = PC->GetLocalPlayer())
			{
				if (UKRUIInputSubsystem* UIInputSubsystem = LP->GetSubsystem<UKRUIInputSubsystem>())
				{
					UIInputSubsystem->ForceResetUIMode();
					UE_LOG(LogMapTravel, Log, TEXT("Reset UIInputSubsystem RefCount"));
				}
			}

			// PlayerController의 InputMode를 게임 모드로 리셋
			PC->SetShowMouseCursor(false);
			FInputModeGameOnly GameOnlyMode;
			PC->SetInputMode(GameOnlyMode);
			UE_LOG(LogMapTravel, Log, TEXT("Reset PlayerController InputMode to GameOnly"));
		}

		// 2-3초 후 Loading 화면 숨기고 레벨 이름 표시
		FTimerHandle LoadingTimerHandle;
		GetGameInstance()->GetTimerManager().SetTimer(
			LoadingTimerHandle,
			[this]()
			{
				// Loading 화면 숨김
				if (UKRUIRouterSubsystem* UIRouter = GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>())
				{
					UIRouter->CloseRoute(FName(TEXT("Loading")));
				}

				// 레벨 이름 표시
				OnLevelLoadComplete(PendingLevelNameKey, PendingLevelSoundTag, PendingDisplayDuration);
			},
			2.5f,  // 2.5초 대기 (2~3초)
			false
		);
	}
}

void UKRMapTravelSubsystem::OnLevelLoadComplete(FName StringTableKey, FGameplayTag SoundTag, float DisplayDuration)
{
	UE_LOG(LogMapTravel, Log, TEXT("Level load complete"));

	// 델리게이트 브로드캐스트
	OnMapTravelCompleted.Broadcast();

	// SaveDeathLevelInfo 메시지 브로드캐스트 (레벨 이름 UI 표시)
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

	FKRUIMessage_Info Message;
	Message.Context = EInfoContext::LevelTransition;
	Message.StringTableKey = StringTableKey.ToString();

	MessageSubsystem.BroadcastMessage(
		FKRUIMessageTags::SaveDeathLevelInfo(),
		Message
	);

	// TODO: 사운드 재생
	if (SoundTag.IsValid())
	{
		UE_LOG(LogMapTravel, Log, TEXT("Playing sound: %s"), *SoundTag.ToString());
		// if (UKRSoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<UKRSoundSubsystem>())
		// {
		//     SoundSubsystem->PlaySound(SoundTag);
		// }
	}

	// 레벨 이름 표시는 자동으로 숨겨짐 (AutoHideDuration)
	// 페이드 인은 별도로 필요 없음 (Loading 화면이 이미 닫혔음)
}

FName UKRMapTravelSubsystem::GetLevelNameKeyFromTag(FGameplayTag ActivationTag)
{
	// 하드코딩된 레벨 이름 매핑 (임시)
	// TODO: 추후 데이터 테이블이나 설정 파일로 이동

	FString TagString = ActivationTag.ToString();

	// 튜토리얼 맵
	if (TagString.Contains(TEXT("Tutorial")))
	{
		return FName(TEXT("Info_Tutorial"));
	}
	// 본부장실 (Office)
	else if (TagString.Contains(TEXT("Office")) || TagString.Contains(TEXT("GeneralManager")))
	{
		return FName(TEXT("Info_Office"));
	}
	// 바리온 메인 맵
	else if (TagString.Contains(TEXT("BarionMain")) || TagString.Contains(TEXT("Barion.Main")))
	{
		return FName(TEXT("Info_BarionMain"));
	}
	// 바리온 항구
	else if (TagString.Contains(TEXT("Harbour")) || TagString.Contains(TEXT("Harbor")))
	{
		return FName(TEXT("Info_Harbour"));
	}
	// 더스트랜드 (Forgotten City)
	else if (TagString.Contains(TEXT("ForgottenCity")) || TagString.Contains(TEXT("Dustlands")))
	{
		return FName(TEXT("Info_ForgottenCity"));
	}
	// 메인 메뉴
	else if (TagString.Contains(TEXT("MainMenu")) || TagString.Contains(TEXT("Menu")))
	{
		return FName(TEXT("Info_MainMenu"));
	}
	// 기본값
	else
	{
		UE_LOG(LogMapTravel, Warning, TEXT("Unknown ActivationTag: %s, using default level name"), *TagString);
		return FName(TEXT("Info_Unknown"));
	}
}
