#include "KRQuestEventTriggerBox.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

AKRQuestEventTriggerBox::AKRQuestEventTriggerBox()
	: bTriggered(false)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	if (UShapeComponent* CollisionComp = GetCollisionComponent())
	{
		CollisionComp->SetVisibleInRayTracing(false);
	}
}

void AKRQuestEventTriggerBox::BeginPlay()
{
	Super::BeginPlay();
}

void AKRQuestEventTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (bTriggered) return;

	AKRHeroCharacter* Player = Cast<AKRHeroCharacter>(OtherActor);

	if (!Player) return;

	if (!EventGameplayTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestEventTriggerBox] EventGameplayTag is not set!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[QuestEventTriggerBox] EventGameplayTag: %s"), *EventGameplayTag.ToString());

	bTriggered = true;

	FQuestEventTriggerBoxMessage Message;
	Message.EventGameplayTag = EventGameplayTag;
	
	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	GameplayMessageSubsystem.BroadcastMessage(EventGameplayTag, Message);
}
