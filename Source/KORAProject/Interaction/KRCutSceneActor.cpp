#include "Interaction/KRCutSceneActor.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "Quest/KRQuestEventTriggerBox.h"
#include "Quest/Condition/QuestTalkNPCChecker.h"
#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Die.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystem/KRInventorySubsystem.h"
#include "SubSystem/KRLoadingSubsystem.h"
#include "Subsystem/KRUIRouterSubsystem.h"

AKRCutSceneActor::AKRCutSceneActor()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetBoxExtent(FVector(100.f));
}

void AKRCutSceneActor::BeginPlay()
{
	Super::BeginPlay();

	if (ActivationObjectiveTag.IsValid())
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
	}

	if (UKRLoadingSubsystem::Get().IsLoadingScreenVisible())
	{
		// 로딩이 끝나면 호출될 함수 등록
		UKRLoadingSubsystem::Get().OnLoadingScreenHidden.AddUObject(this, &AKRCutSceneActor::OnLoadingScreenHidden);
	}
	
	if (ActivationObjectiveTag.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		
		if (ActivationObjectiveTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Enemy"))))
		{
			ListenerHandle = MessageSubsystem.RegisterListener<FKillMonsterMessage>(
				ActivationObjectiveTag,
				this,
				&AKRCutSceneActor::OnKilledMonster
			);
		}
		else if (ActivationObjectiveTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("ItemType.Quest"))))
		{
			ListenerHandle = MessageSubsystem.RegisterListener<FAddItemMessage>(
				ActivationObjectiveTag,
				this,
				&AKRCutSceneActor::OnItemAdded
			);
		}
		else if (ActivationObjectiveTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Event.ForgottenCity"))))
		{
			ListenerHandle = MessageSubsystem.RegisterListener<FQuestEventTriggerBoxMessage>(
				ActivationObjectiveTag,
				this,
				&AKRCutSceneActor::OnLocationEntered
			);
		}
		else if (ActivationObjectiveTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("NPC"))))
		{
			ListenerHandle = MessageSubsystem.RegisterListener<FQuestNPCDialogueCompleteMessage>(
				ActivationObjectiveTag,
				this,
				&AKRCutSceneActor::OnNPCDialogueCompleted
			);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Portal] Unknown ObjectiveTag category: %s"), *ActivationObjectiveTag.ToString());
		}
	}
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AKRCutSceneActor::OnTriggerBeginOverlap);
}

void AKRCutSceneActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
		MessageSubsystem.UnregisterListener(ListenerHandle);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AKRCutSceneActor::OnKilledMonster(FGameplayTag Channel, const struct FKillMonsterMessage& Message)
{
	ActivateCutSceneIfMatch(Channel);
}

void AKRCutSceneActor::OnItemAdded(FGameplayTag Channel, const struct FAddItemMessage& Message)
{
	if (Message.ItemTag.MatchesTagExact(ActivationObjectiveTag))
	{
		ActivateCutSceneIfMatch(Channel);
	}
}

void AKRCutSceneActor::OnLocationEntered(FGameplayTag Channel, const struct FQuestEventTriggerBoxMessage& Message)
{
	ActivateCutSceneIfMatch(Channel);
}

void AKRCutSceneActor::OnNPCDialogueCompleted(FGameplayTag Channel,
	const struct FQuestNPCDialogueCompleteMessage& Message)
{
	ActivateCutSceneIfMatch(Channel);
}

void AKRCutSceneActor::OnTriggerBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (UKRLoadingSubsystem::Get().IsLoadingScreenVisible())
		return;
	
	if (!OtherActor || !OtherActor->ActorHasTag(TEXT("Player")))
	{
		return;
	}

	if (RouteToOpen.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CutSceneActor] RouteToOpen is None"));
		Destroy();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UKRUIRouterSubsystem* Router =
				GI->GetSubsystem<UKRUIRouterSubsystem>())
			{
				Router->ToggleRoute(RouteToOpen);
			}
		}
	}

	Destroy();
}

void AKRCutSceneActor::OnLoadingScreenHidden()
{
	AActor* PlayerActor = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerActor || !TriggerBox->IsOverlappingActor(PlayerActor))
		return;

	if (RouteToOpen.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CutSceneActor] RouteToOpen is None"));
		Destroy();
		return;
	}

	// ✅ 실제 컷씬 실행 부분 (기존과 동일하게 사용)
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
			{
				Router->ToggleRoute(RouteToOpen);
			}
		}
	}

	Destroy();
}

void AKRCutSceneActor::ActivateCutSceneIfMatch(FGameplayTag Channel)
{
	if (bIsActivated)
		return;

	if (!Channel.MatchesTagExact(ActivationObjectiveTag))
		return;

	bIsActivated = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	UE_LOG(LogTemp, Warning, TEXT("[Portal] Activated by tag: %s"), *Channel.ToString());
}
