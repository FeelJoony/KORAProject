#include "AbilityTask_GrantNearbyInteraction.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Interaction/InteractableTarget.h"
#include "Interaction/InteractionStatics.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/OverlapResult.h"

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(
	UGameplayAbility* OwningAbility,
	float InteractionScanRange,
	float InteractionScanRate)
{
	UAbilityTask_GrantNearbyInteraction* MyObj = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	MyObj->InteractionScanRange = InteractionScanRange;
	MyObj->InteractionScanRate = InteractionScanRate;
	return MyObj;
}

void UAbilityTask_GrantNearbyInteraction::Activate()
{
	Super::Activate();

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(QueryTimerHandle, this, &UAbilityTask_GrantNearbyInteraction::QueryInteractables, InteractionScanRate, true);
	}
}

void UAbilityTask_GrantNearbyInteraction::OnDestroy(bool AbilityEnded)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(QueryTimerHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

void UAbilityTask_GrantNearbyInteraction::QueryInteractables()
{
	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	// Perform a sphere overlap to find nearby interactables
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	World->OverlapMultiByChannel(
		Overlaps,
		AvatarActor->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(InteractionScanRange),
		QueryParams
	);

	TArray<FInteractionOption> InteractionOptions;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* OverlappedActor = Overlap.GetActor();
		if (!OverlappedActor)
		{
			continue;
		}

		IInteractableTarget* InteractableTarget = Cast<IInteractableTarget>(OverlappedActor);
		if (InteractableTarget)
		{
			FInteractionQuery Query;
			Query.RequestingActor = AvatarActor;
			Query.RequestingController = AvatarActor->GetInstigatorController();

			if (InteractableTarget->Execute_CanInteract(OverlappedActor, Query))
			{
				TArray<FInteractionOption> Options;
				InteractableTarget->Execute_GatherInteractionOptions(OverlappedActor, Query, Options);
				InteractionOptions.Append(Options);
			}
		}
	}

	UpdateInteractionAbility(InteractionOptions);
}

void UAbilityTask_GrantNearbyInteraction::UpdateInteractionAbility(const TArray<FInteractionOption>& InteractionOptions)
{
	if (!AbilitySystemComponent.IsValid() || !InteractionAbilityToGrant)
	{
		return;
	}

	// If we have interactions and no ability is granted yet
	if (InteractionOptions.Num() > 0 && GrantedAbilityHandles.Num() == 0)
	{
		FGameplayAbilitySpec AbilitySpec(InteractionAbilityToGrant, 1, INDEX_NONE, this);
		FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(AbilitySpec);
		GrantedAbilityHandles.Add(Handle);
		OnInteractionGranted.Broadcast(Handle);
	}
	// If we have no interactions but ability is granted
	else if (InteractionOptions.Num() == 0 && GrantedAbilityHandles.Num() > 0)
	{
		for (const FGameplayAbilitySpecHandle& Handle : GrantedAbilityHandles)
		{
			if (Handle.IsValid())
			{
				AbilitySystemComponent->ClearAbility(Handle);
				OnInteractionRemoved.Broadcast(Handle);
			}
		}
		GrantedAbilityHandles.Empty();
	}
}