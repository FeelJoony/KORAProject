#include "Interaction/InteractableActorBase.h"

#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Subsystem/KRUIRouterSubsystem.h"
#include "Characters/KRHeroCharacter.h"

AInteractableActorBase::AInteractableActorBase()
{
	InteractCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractCollision"));
	InteractCollision->SetupAttachment(RootComponent);
	InteractCollision->SetSphereRadius(300.f);
	InteractCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void AInteractableActorBase::BeginPlay()
{
	Super::BeginPlay();

	InteractCollision->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActorBase::OnOverlapBegin);
	InteractCollision->OnComponentEndOverlap.AddDynamic(this, &AInteractableActorBase::OnOverlapEnd);
}

void AInteractableActorBase::GiveTagToActor(AActor* TargetActor, FGameplayTag Tag)
{
	if (!IsValid(TargetActor) || !Tag.IsValid())
	{
		return;
	}

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor))
	{
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
		if (ASC && InteractAbilityTag.IsValid())
		{
			ASC->AddLooseGameplayTag(InteractAbilityTag);
			ShowInteractionUI();
		}
	}
}

void AInteractableActorBase::RemoveTagFromActor(AActor* TargetActor, FGameplayTag Tag)
{
	if (!IsValid(TargetActor) || !Tag.IsValid())
	{
		return;
	}

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor))
	{
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
		if (ASC && InteractAbilityTag.IsValid())
		{
			ASC->RemoveLooseGameplayTag(InteractAbilityTag);
			HideInteractionUI();
		}
	}
}

void AInteractableActorBase::OnActorEnterRange(AActor* TargetActor)
{
	if (AKRHeroCharacter* Character = Cast<AKRHeroCharacter>(TargetActor))
	{
		if (!IsValid(Character)) return;

		GiveTagToActor(Character, InteractAbilityTag);
	}
}

void AInteractableActorBase::OnActorExitRange(AActor* TargetActor)
{
	if (AKRHeroCharacter* Character = Cast<AKRHeroCharacter>(TargetActor))
	{
		if (!IsValid(Character)) return;

		RemoveTagFromActor(Character, InteractAbilityTag);
	}
}

void AInteractableActorBase::ShowInteractionUI()
{

}

void AInteractableActorBase::HideInteractionUI()
{

}

void AInteractableActorBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 BodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	//interact actor의 tag를 player에게 전달
	if (!IsValid(OtherActor)) return;

	OnActorEnterRange(OtherActor);
}

void AInteractableActorBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 BodyIndex)
{
	if (!IsValid(OtherActor)) return;

	//loose tag
	OnActorExitRange(OtherActor);
}
