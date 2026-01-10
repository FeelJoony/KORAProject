#include "KRQuestSubsystem.h"

#include "KRDataTablesSubsystem.h"
#include "KRInventorySubsystem.h"
#include "Data/QuestDataStruct.h"
#include "Quest/KRQuestActor.h"
#include "Quest/KRQuestDataDefinition.h"
#include "Quest/KRQuestInstance.h"
#include "Quest/Condition/QuestAddItemChecker.h"
#include "Quest/Condition/PlayAbilityChecker.h"
#include "Quest/Condition/KillMonsterChecker.h"
#include "Quest/Condition/QuestTalkNPCChecker.h"
#include "Quest/Delegates/QuestDelegate.h"
#include "System/KRAssetManager.h"
#include "Quest/Delegates/TutorialDelegate.h"
#include "Quest/Delegates/MainQuestDelegate.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KREnemyTag.h"
#include "GameplayTag/KRNPCTag.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Quest/Condition/QuestEnterLocationChecker.h"
#include "Subsystem/KRUIRouterSubsystem.h"

DEFINE_LOG_CATEGORY(LogQuestSubSystem);

void UKRQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<UKRDataTablesSubsystem>();
	Collection.InitializeDependency<UKRInventorySubsystem>();

	QuestDelegateMap.Add(EQuestType::Q_Tutorial, NewObject<UTutorialDelegate>(this));
	QuestDelegateMap.Add(EQuestType::Q_Main, NewObject<UMainQuestDelegate>(this));

	CheckerGroup.Add(KRTAG_ABILITY_LOCKON, UPlayAbilityChecker::StaticClass());
	CheckerGroup.Add(KRTAG_STATE_ACTING_LOCKON, UPlayAbilityChecker::StaticClass());
	CheckerGroup.Add(KRTAG_STATE_ACTING_GRAPPLING_SUCCESS, UPlayAbilityChecker::StaticClass());
	CheckerGroup.Add(KRTAG_ENEMY_AISTATE_DEAD, UKillMonsterChecker::StaticClass());
	
	CheckerGroup.Add(KRTAG_NPC_TYPE_CLARA, UQuestTalkNPCChecker::StaticClass());
	CheckerGroup.Add(KRTAG_EVENT_FORGOTTENCITY_MISSIONAREA, UQuestEnterLocationChecker::StaticClass());
	CheckerGroup.Add(KRTAG_EVENT_FORGOTTENCITY_ACCESSPOINT, UQuestEnterLocationChecker::StaticClass());
	CheckerGroup.Add(KRTAG_ITEMTYPE_QUEST_COREWEAPONBLUEPRINT, UQuestAddItemChecker::StaticClass());
	CheckerGroup.Add(KRTAG_EVENT_FORGOTTENCITY_BOSSARENA, UQuestEnterLocationChecker::StaticClass());
	CheckerGroup.Add(KRTAG_EVENT_FORGOTTENCITY_FINALBOSSARENA, UQuestEnterLocationChecker::StaticClass());
	
}

void UKRQuestSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

UKRQuestSubsystem& UKRQuestSubsystem::Get(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	check(World);
	UKRQuestSubsystem* QuestSubsystem = UGameInstance::GetSubsystem<UKRQuestSubsystem>(World->GetGameInstance());
	check(QuestSubsystem);
	return *QuestSubsystem;
}

void UKRQuestSubsystem::AcceptQuest(int32 Index)
{
	AKRQuestActor* QuestActor = GetWorld()->SpawnActorDeferred<AKRQuestActor>(LoadQuestActorClass(Index), FTransform::Identity);

	UKRQuestInstance* QuestInstance = NewObject<UKRQuestInstance>(this);
	if (QuestInstance)
	{
		QuestInstance->Initialize(Index);
		
		ActiveInstance = QuestInstance;
		ActiveActor = QuestActor;
		QuestActor->SetQuestInstance(QuestInstance);
	}
	
	QuestActor->FinishSpawning(FTransform::Identity);
}

bool UKRQuestSubsystem::AbandonQuest(int32 Index)
{
	return false;
}

void UKRQuestSubsystem::RespawnQuestActorForLevelTransition()
{
	if (!ActiveInstance) return;
	if (ActiveInstance->GetQuestState() != EQuestState::InProgress) return;
	if (IsValid(ActiveActor)) return;
	
	const int32 QuestIndex = ActiveInstance->GetQuestIndex();
	TSubclassOf<AKRQuestActor> QuestActorClass = LoadQuestActorClass(QuestIndex);
	if (!QuestActorClass) return;
	UWorld* World = GetWorld();
	if (!World) return;

	AKRQuestActor* NewQuestActor = World->SpawnActorDeferred<AKRQuestActor>(QuestActorClass, FTransform::Identity);
	if (!NewQuestActor) return;

	NewQuestActor->SetQuestInstance(ActiveInstance);
	ActiveActor = NewQuestActor;
	NewQuestActor->FinishSpawning(FTransform::Identity);
	ReinitializeQuestDelegate(ActiveInstance);
}

void UKRQuestSubsystem::NotifyObjectiveEvent(FGameplayTag TargetTag, int32 Count)
{
	
}

void UKRQuestSubsystem::NotifyNPCDialogueComplete(FGameplayTag NPCTag)
{
	if (!NPCTag.IsValid())
	{
		return;
	}
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

	FQuestNPCDialogueCompleteMessage Message;
	Message.NPCTag = NPCTag;

	MessageSubsystem.BroadcastMessage(NPCTag, Message);
}

void UKRQuestSubsystem::MarkNPCDialogueForQuest(FGameplayTag NPCTag)
{
	if (!NPCTag.IsValid())
	{
		return;
	}

	const int32 CurrentQuestIdx = GetCurrentQuestIndex();
	if (CurrentQuestIdx == INDEX_NONE)
	{
		return;
	}
	FQuestDialogueNPCSet& NPCSet = QuestDialogueNPCs.FindOrAdd(CurrentQuestIdx);
	NPCSet.NPCTags.Add(NPCTag);
}

bool UKRQuestSubsystem::HasTalkedToNPCForCurrentQuest(FGameplayTag NPCTag) const
{
	const int32 CurrentQuestIdx = GetCurrentQuestIndex();
	if (CurrentQuestIdx == INDEX_NONE)
	{
		return false;
	}

	const FQuestDialogueNPCSet* NPCSet = QuestDialogueNPCs.Find(CurrentQuestIdx);
	if (!NPCSet)
	{
		return false;
	}

	return NPCSet->NPCTags.Contains(NPCTag);
}

int32 UKRQuestSubsystem::GetCurrentQuestIndex() const
{
	if (ActiveInstance)
	{
		return ActiveInstance->GetQuestIndex();
	}
	return INDEX_NONE;
}

void UKRQuestSubsystem::InitializeQuestDelegate(class UKRQuestInstance* NewQuestInstance)
{
	for (const auto& QuestDelegate : QuestDelegateMap)
	{
		QuestDelegate.Value->Initialize(NewQuestInstance);
	}
}

void UKRQuestSubsystem::ReinitializeQuestDelegate(class UKRQuestInstance* NewQuestInstance)
{
	for (const auto& QuestDelegate : QuestDelegateMap)
	{
		QuestDelegate.Value->Reinitialize(NewQuestInstance);
	}
}

void UKRQuestSubsystem::UninitializeQuestDelegate()
{
	for (const auto& QuestDelegate : QuestDelegateMap)
	{
		QuestDelegate.Value->Uninitialize();
	}
}

void UKRQuestSubsystem::OnAcceptedBroadcast(EQuestType Type, int32 OrderIndex)
{
	if (UQuestDelegate* QuestDelegate = QuestDelegateMap.FindRef(Type))
	{
		QuestDelegate->OnAcceptedBroadcast(OrderIndex);
	}
}

void UKRQuestSubsystem::OnCompletedBroadcast(EQuestType Type, int32 OrderIndex)
{
	if (UQuestDelegate* QuestDelegate = QuestDelegateMap.FindRef(Type))
	{
		QuestDelegate->OnCompletedBroadcast(OrderIndex);
	}
	
	const int32 QuestIndex = GetCurrentQuestIndex();
	TriggerQuestCutscene(QuestIndex, OrderIndex);
}

void UKRQuestSubsystem::OnFailedBroadcast(EQuestType Type, int32 OrderIndex)
{
	if (UQuestDelegate* QuestDelegate = QuestDelegateMap.FindRef(Type))
	{
		QuestDelegate->OnFailedBroadcast(OrderIndex);
	}
}

void UKRQuestSubsystem::OnSetNextSubBroadcast(EQuestType Type, int32 OrderIndex)
{
	if (UQuestDelegate* QuestDelegate = QuestDelegateMap.FindRef(Type))
	{
		QuestDelegate->OnSetNextSubBroadcast(OrderIndex);
	}
}

void UKRQuestSubsystem::OnProgressBroadcast(EQuestType Type, int32 OrderIndex, const struct FSubQuestEvalData& EvalData)
{
	if (UQuestDelegate* QuestDelegate = QuestDelegateMap.FindRef(Type))
	{
		QuestDelegate->OnProgressBroadcast(OrderIndex, EvalData);
	}
}

void UKRQuestSubsystem::TriggerQuestCutscene(int32 QuestIndex, int32 OrderIndex)
{
	FName CutsceneRoute = NAME_None;
	
	if (QuestIndex == 2 && OrderIndex == 6)
	{
		CutsceneRoute = FName("SemiBoss");
	}
	else if (QuestIndex == 2 && OrderIndex == 8)
	{
		CutsceneRoute = FName("Ending");
	}

	if (CutsceneRoute != NAME_None)
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
			{
				Router->ToggleRoute(CutsceneRoute);
			}
		}
	}
}

TSubclassOf<AKRQuestActor> UKRQuestSubsystem::LoadQuestActorClass(int32 Index)
{
	UKRDataTablesSubsystem& DataTableSubsystem = UKRDataTablesSubsystem::Get(this);
	if (FQuestDataStruct* QuestData = DataTableSubsystem.GetData<FQuestDataStruct>(Index))
	{
		FPrimaryAssetId PrimaryAssetId;
		if (!PrimaryAssetId.IsValid())
		{
			PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetId(FPrimaryAssetType("KRQuestDataDefinition"), QuestData->StateTreeDefinitionPath));

			checkf(PrimaryAssetId.IsValid(), TEXT("PrimaryAssetId is invalid"));

			UKRAssetManager& AssetManager = UKRAssetManager::Get();

			TSubclassOf<UKRQuestDataDefinition> AssetClass;
			{
				FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(PrimaryAssetId);
				AssetClass = Cast<UClass>(AssetPath.TryLoad());
			}

			if (!AssetClass)
			{
				UE_LOG(LogQuestSubSystem, Error, TEXT("Failed to load QuestDataDefinition class from asset path"));
				return nullptr;
			}

			UKRQuestDataDefinition* QuestDataDefinition = AssetClass->GetDefaultObject<UKRQuestDataDefinition>();
			check(QuestDataDefinition);
			check(QuestDataDefinition->QuestActor);

			return QuestDataDefinition->QuestActor;
		}
	}

	return nullptr;
}
