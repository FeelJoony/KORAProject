#include "Interaction/InteractableActorBase.h"

#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "Subsystem/KRUIRouterSubsystem.h"
#include "Characters/KRHeroCharacter.h"
#include "Player/KRPlayerController.h"

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

	AKRHeroCharacter* Player = Cast<AKRHeroCharacter>(TargetActor);
	if (!IsValid(Player)) return;

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Player))
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
	if (AKRBaseCharacter* Character = Cast<AKRBaseCharacter>(TargetActor))
	{
		if (!IsValid(Character)) return;

		if (ObservedASC && AbilityActivatedHandle.IsValid())
		{
			ObservedASC->AbilityActivatedCallbacks.Remove(AbilityActivatedHandle);
			AbilityActivatedHandle.Reset();
			ObservedASC = nullptr;
		}

		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Character))
		{
			UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
			if (ASC)
			{
				ObservedASC = ASC;

				AbilityActivatedHandle =
					ObservedASC->AbilityActivatedCallbacks.AddUObject(
						this,
						&AInteractableActorBase::OnAbilityActivated
					);
			}
		}

		if (AKRPlayerController* PC = Cast<AKRPlayerController>(Character->GetController()))
		{
			PC->SetCurrentInteractableActor(this);
		}

		GiveTagToActor(Character, InteractAbilityTag);
	}
}

void AInteractableActorBase::OnActorExitRange(AActor* TargetActor)
{
	if (AKRHeroCharacter* Character = Cast<AKRHeroCharacter>(TargetActor))
	{
		if (!IsValid(Character)) return;

		if (AKRPlayerController* PC = Cast<AKRPlayerController>(Character->GetController()))
		{
			if (PC->GetCurrentInteractableActor() == this)
			{
				PC->SetCurrentInteractableActor(nullptr);
			}
		}

		RemoveTagFromActor(Character, InteractAbilityTag);
	}
}

void AInteractableActorBase::ShowInteractionUI()
{
	UKRUIRouterSubsystem* Router = GetWorld()->GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>();
	if (IsValid(Router))
	{
		Router->ToggleRoute("ScreenInteract");
	}
}

void AInteractableActorBase::HideInteractionUI()
{
	UKRUIRouterSubsystem* Router = GetWorld()->GetGameInstance()->GetSubsystem<UKRUIRouterSubsystem>();
	if (IsValid(Router))
	{
		Router->CloseRoute("ScreenInteract");
	}
}

void AInteractableActorBase::EndInteractActor()
{
}

void AInteractableActorBase::OnAbilityActivated(UGameplayAbility* Ability)
{
	if (!Ability || !InteractAbilityTag.IsValid()) return;
	
	const FGameplayTagContainer& AbilityTags = Ability->GetAssetTags();

	if (AbilityTags.HasTagExact(InteractAbilityTag))
	{
		bIsInteract = true;

		OnAbilityActivateBP.Broadcast(Ability);
	}
}

void AInteractableActorBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 BodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor)) return;

	OnActorEnterRange(OtherActor);
}

void AInteractableActorBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 BodyIndex)
{
	if (!IsValid(OtherActor)) return;

	OnActorExitRange(OtherActor);
}
 