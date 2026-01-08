#include "Interaction/WorldAreaVolume.h"
#include "Components/BoxComponent.h"
#include "Components/KRWorldEventComponent.h"
#include "Characters/KRHeroCharacter.h"

AWorldAreaVolume::AWorldAreaVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);

	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AWorldAreaVolume::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWorldAreaVolume::OnBeginOverlap);
}

void AWorldAreaVolume::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AKRHeroCharacter* Player = Cast<AKRHeroCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	UKRWorldEventComponent* WorldEventComp =
		Player->FindComponentByClass<UKRWorldEventComponent>();

	if (!WorldEventComp)
	{
		return;
	}

	if (AreaVolumeTag.IsValid())
	{
		WorldEventComp->TriggerWorldEvent(
			AreaVolumeTag,
			this
		);
	}
}