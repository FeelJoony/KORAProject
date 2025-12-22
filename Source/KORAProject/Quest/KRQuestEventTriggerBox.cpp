#include "KRQuestEventTriggerBox.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/ShapeComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

AKRQuestEventTriggerBox::AKRQuestEventTriggerBox()
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

	UGameplayMessageSubsystem& GameplayMessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
	//GameplayMessageSubsystem.BroadcastMessage(EventGameplayTag, )
}

void AKRQuestEventTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (AKRHeroCharacter* Character = Cast<AKRHeroCharacter>(OtherActor))
	{
		
	}
}
