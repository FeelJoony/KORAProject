#include "Interaction/KRPortalActor.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "SubSystem/KRMapTravelSubsystem.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Characters/KRHeroCharacter.h"
#include "Subsystem/KRInventorySubsystem.h"
#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Die.h"
#include "Quest/KRQuestEventTriggerBox.h"
#include "Quest/Condition/QuestTalkNPCChecker.h"

AKRPortalActor::AKRPortalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 컴포넌트 생성
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	RootComponent = PortalMesh;

	PortalVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PortalVFX"));
	PortalVFX->SetupAttachment(RootComponent);
	PortalVFX->SetAutoActivate(false);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
}

void AKRPortalActor::BeginPlay()
{
	Super::BeginPlay();

	// 초기에는 비활성화
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	
	if (ActivationObjectiveTag.IsValid())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		
		if (ActivationObjectiveTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Enemy"))))
		{
			ListenerHandle = MessageSubsystem.RegisterListener<FKillMonsterMessage>(
				ActivationObjectiveTag,
				this,
				&AKRPortalActor::OnKilledMonster
			);
		}
		else if (ActivationObjectiveTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("ItemType.Quest"))))
		{
			ListenerHandle = MessageSubsystem.RegisterListener<FAddItemMessage>(
				ActivationObjectiveTag,
				this,
				&AKRPortalActor::OnItemAdded
			);
		}
		else if (ActivationObjectiveTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Event.ForgottenCity"))))
		{
			ListenerHandle = MessageSubsystem.RegisterListener<FQuestEventTriggerBoxMessage>(
				ActivationObjectiveTag,
				this,
				&AKRPortalActor::OnLocationEntered
			);
		}
		else if (ActivationObjectiveTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("NPC"))))
		{
			ListenerHandle = MessageSubsystem.RegisterListener<FQuestNPCDialogueCompleteMessage>(
				ActivationObjectiveTag,
				this,
				&AKRPortalActor::OnNPCDialogueCompleted
			);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[Portal] Unknown ObjectiveTag category: %s"), *ActivationObjectiveTag.ToString());
		}
	}

	// 트리거 바인딩
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AKRPortalActor::OnTriggerBeginOverlap);
}

void AKRPortalActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
		MessageSubsystem.UnregisterListener(ListenerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void AKRPortalActor::OnKilledMonster(FGameplayTag Channel, const struct FKillMonsterMessage& Message)
{
	ActivatePortalIfMatch(Channel);
}

void AKRPortalActor::OnItemAdded(FGameplayTag Channel, const struct FAddItemMessage& Message)
{
	if (Message.ItemTag.MatchesTagExact(ActivationObjectiveTag))
	{
		ActivatePortalIfMatch(Channel);
	}
}

void AKRPortalActor::OnLocationEntered(FGameplayTag Channel, const struct FQuestEventTriggerBoxMessage& Message)
{
	ActivatePortalIfMatch(Channel);
}

void AKRPortalActor::OnNPCDialogueCompleted(FGameplayTag Channel, const struct FQuestNPCDialogueCompleteMessage& Message)
{
	ActivatePortalIfMatch(Channel);
}

void AKRPortalActor::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActivated)
	{
		return;
	}

	AKRHeroCharacter* Player = Cast<AKRHeroCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Portal] Player entered portal!"));
	UKRMapTravelSubsystem::Get(this).TravelToExperience(TargetUserFacingPath.ToString());
}

void AKRPortalActor::ActivatePortalIfMatch(FGameplayTag Channel)
{
	if (bIsActivated)
		return;

	if (!Channel.MatchesTagExact(ActivationObjectiveTag))
		return;

	bIsActivated = true;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	if (PortalVFX) PortalVFX->Activate();

	UE_LOG(LogTemp, Warning, TEXT("[Portal] Activated by tag: %s"), *Channel.ToString());
}