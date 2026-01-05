#include "KRQuestEventTriggerBox.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

AKRQuestEventTriggerBox::AKRQuestEventTriggerBox()
	: bTriggered(false)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	// TriggerBox는 보이지 않으므로 Ray Tracing 불필요
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


	if (AKRHeroCharacter* Character = Cast<AKRHeroCharacter>(OtherActor))
	{
		if (!EventGameplayTag.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[QuestEventTriggerBox] EventGameplayTag is not set!"));
			return;
		}

		bTriggered = true;

		FQuestEventTriggerBoxMessage Message;
		Message.EventGameplayTag = EventGameplayTag;
		UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		GameplayMessageSubsystem.BroadcastMessage(EventGameplayTag, Message);
	}
}
