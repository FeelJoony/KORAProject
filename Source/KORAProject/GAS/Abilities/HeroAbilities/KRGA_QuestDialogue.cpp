#include "KRGA_QuestDialogue.h"
#include "AbilitySystemComponent.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRQuestSubsystem.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "Interaction/KRNPCInteractActor.h"
#include "Player/KRPlayerController.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KREventTag.h"
#include "Data/GameDataType.h"
#include "GameFramework/GameplayMessageSubsystem.h"

UKRGA_QuestDialogue::UKRGA_QuestDialogue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_NPCINTERACT_CLARA);
	SetAssetTags(Tags);
}

void UKRGA_QuestDialogue::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UWorld* World = ActorInfo->AvatarActor->GetWorld();
	if (!World)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	AKRNPCInteractActor* NPCActor = GetNPCFromPlayerController(ActorInfo);
	if (!NPCActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KRGA_QuestDialogue] No NPC found from PlayerController"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FGameplayTag NPCTag = NPCActor->GetNPCTag();
	const int32 DialogueIndex = NPCActor->GetCurrentDialogueIndex();

	UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(World);
	const bool bAlreadyTalked = QuestSubsystem.HasTalkedToNPCForCurrentQuest(NPCTag);

	UKRUIRouterSubsystem* Router = World->GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>();
	if (IsValid(Router) && !Router->IsRouteOpen(DialogueRouteName))
	{
		Router->OpenRoute(DialogueRouteName);
	}

	FDialogueDataStruct DialogueData;

	if (bAlreadyTalked)
	{
		const int32 QuestIndex = QuestSubsystem.GetCurrentQuestIndex();
		DialogueData.NPCTag = NPCTag;
		DialogueData.DialogueTextKeys.Add(MakeAlreadyTalkedKey(QuestIndex));
	}
	else
	{
		DialogueData = LoadDialogueData(DialogueIndex, NPCTag);
	}

	UGameplayMessageSubsystem::Get(World).BroadcastMessage(KRTAG_EVENT_DIALOGUE_START, DialogueData);
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UKRGA_QuestDialogue::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AKRNPCInteractActor* UKRGA_QuestDialogue::GetNPCFromPlayerController(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return nullptr;
	}

	APawn* AvatarPawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
	if (!AvatarPawn)
	{
		return nullptr;
	}

	AKRPlayerController* PC = Cast<AKRPlayerController>(AvatarPawn->GetController());
	if (!PC)
	{
		return nullptr;
	}

	AInteractableActorBase* InteractableActor = PC->GetCurrentInteractableActor();
	return Cast<AKRNPCInteractActor>(InteractableActor);
}

FDialogueDataStruct UKRGA_QuestDialogue::LoadDialogueData(int32 DialogueIndex, FGameplayTag NPCTag) const
{
	FDialogueDataStruct Result;
	Result.NPCTag = NPCTag;

	if (DialogueIndex < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KRGA_QuestDialogue] DialogueIndex is invalid: %d"), DialogueIndex);
		return Result;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return Result;
	}

	UKRDataTablesSubsystem* DataTableSubsystem = World->GetGameInstance()->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataTableSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[KRGA_QuestDialogue] DataTableSubsystem not found"));
		return Result;
	}

	FDialogueDataStruct* DialogueData = DataTableSubsystem->GetDataSafe<FDialogueDataStruct>(DialogueIndex);
	if (DialogueData)
	{
		Result = *DialogueData;
		Result.NPCTag = NPCTag;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[KRGA_QuestDialogue] Failed to load DialogueData at index: %d"), DialogueIndex);
	}

	return Result;
}

FName UKRGA_QuestDialogue::MakeAlreadyTalkedKey(int32 QuestIndex) const
{
	return FName(*FString::Printf(TEXT("DLG_%d_Talked"), QuestIndex));
}
