#include "KRGA_Interact.h"
#include "GAS/Tasks/AbilityTask_WaitForInteractableTargets_SingleLineTrace.h"
#include "Interaction/InteractableTarget.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GAS/Tasks/AbilityTask_GrantNearbyInteraction.h"

UKRGA_Interact::UKRGA_Interact()
{
	LineTraceProfile = FCollisionProfileName(TEXT("Pawn"));
	InteractionRange = 500.f;
	InteractionScanRate = 0.1f;
	bShowDebug = false;
}

void UKRGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	

	UAbilityTask_WaitForInteractableTargets_SingleLineTrace* WaitTask =
		UAbilityTask_WaitForInteractableTargets_SingleLineTrace::WaitForInteractableTargets_SingleLineTrace(
			this,
			InteractionRange,
			InteractionScanRate,
			bShowLineTraceDebug
		);
	//WaitTask->NewOption.AddDynamic(this, &UKRGA_Interact::OnLineTraceTargetsChanged);
	WaitTask->ReadyForActivation();

	UAbilityTask_GrantNearbyInteraction* GrantTask =
		UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(
			this,
			InteractionRange,
			InteractionScanRate
		);
	//GrantTask->OnInteractionGranted.AddDynamic(this, &UKRGA_Interact::OnNearbyInteractionGranted);
	//GrantTask->OnInteractionRemoved.AddDynamic(this, &UKRGA_Interact::OnNearbyInteractionRemoved);
	GrantTask->ReadyForActivation();
	
	
	// If we have current options, trigger the first one
	if (CurrentOptions.Num() > 0)
	{
		TriggerInteraction();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		// No current interactions available
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UKRGA_Interact::OnInteractableTargetsChanged(const TArray<FInteractionOption>& InteractableOptions)
{
	CurrentOptions = InteractableOptions;

	// Here you could update UI to show available interactions
	// For now, we just store them
}

void UKRGA_Interact::TriggerInteraction()
{
	if (CurrentOptions.Num() == 0)
	{
		return;
	}

	// Take the first available interaction
	FInteractionOption& Option = CurrentOptions[0];

	if (Option.InteractableTarget.GetObject())
	{
		AActor* TargetActor = Cast<AActor>(Option.InteractableTarget.GetObject());
		if (TargetActor)
		{
			IInteractableTarget* InteractableTarget = Cast<IInteractableTarget>(TargetActor);
			if (InteractableTarget)
			{
				AActor* AvatarActor = GetAvatarActorFromActorInfo();
				InteractableTarget->Execute_OnInteractionTriggered(TargetActor, AvatarActor);
			}
		}
	}
}
