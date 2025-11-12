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
	
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	World->OverlapMultiByProfile(
		OverlapResults,
		AvatarActor->GetActorLocation(),
		FQuat::Identity,
		FName(TEXT("Interact_Sphere")),
		FCollisionShape::MakeSphere(InteractionScanRange),
		QueryParams
	);
	
	if (OverlapResults.Num() <= 0)
	{
		return;
	}

	TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
	
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		TScriptInterface<IInteractableTarget> InteractableActor(Overlap.GetActor());
		if (InteractableActor)
		{
			InteractableTargets.AddUnique(InteractableActor);
		}

		TScriptInterface<IInteractableTarget> InteractableComponent(Overlap.GetActor());
		if (InteractableComponent)
		{
			InteractableTargets.AddUnique(InteractableComponent);
		}
	}

	FInteractionQuery InteractionQuery;
	InteractionQuery.RequestingActor = AvatarActor;
	InteractionQuery.RequestingController=Cast<AController>(AvatarActor->GetOwner());

// Option갱신 함수 분리 예정
	
	TArray<FInteractionOption> Options;
	for (TScriptInterface<IInteractableTarget>& InteractableTarget : InteractableTargets)
	{
		FInteractionOptionBuilder InteractionBuilder(InteractableTarget, Options);
		InteractableTarget->GatherInteractionOptions(InteractionQuery, InteractionBuilder);
	}

	for (const FInteractionOption& Option : Options)
	{
		if (Option.InteractionAbilityToGrant)
		{
			FObjectKey ObjectKey(Option.InteractionAbilityToGrant);
			if (!InteractionAbilityCache.Find(ObjectKey))
			{
				FGameplayAbilitySpec Spec(Option.InteractionAbilityToGrant,1,INDEX_NONE,this);
				FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
				InteractionAbilityCache.Add(ObjectKey,Handle);
			}
		}
	}

	bool bOptionChanged = false;
	if (Options.Num() != CurrentOptions.Num())
	{
		bOptionChanged = true;
	}
	else
	{
		for (int32 i = 0; i < Options.Num(); ++i)
		{
			if (Options[i] != CurrentOptions[i])
			{
				bOptionChanged = true;
				break;
			}
		}
	}

	if (bOptionChanged)
	{
		CurrentOptions=Options;
		InteractionChanged.Broadcast(CurrentOptions);
	}
	//AddToViewport(Option[0]);
}

void UAbilityTask_GrantNearbyInteraction::UpdateInteractableOptions(
	const FInteractionQuery& InInteractionQuery, const TScriptInterface<IInteractableTarget>& InteractableTarget)
{
	TArray<FInteractionOption> NewOptions;
	TArray<FInteractionOption> TempOptions;

	//중복 내용
	FInteractionOptionBuilder InteractionBuilder(InteractableTarget, TempOptions);
	InteractableTarget->GatherInteractionOptions(InInteractionQuery, InteractionBuilder);

	//TargetASC가 있는지 확인 후 Option에 대입
	FGameplayAbilitySpec* InteractionAbilitySpec = nullptr;
	for (FInteractionOption& Option : TempOptions)
	{
		if (Option.TargetASC && Option.TargetInteractionAbilityHandle.IsValid())
		{
			InteractionAbilitySpec = Option.TargetASC->FindAbilitySpecFromHandle(Option.TargetInteractionAbilityHandle);
		}
		else if (Option.InteractionAbilityToGrant)
		{
			InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Option.InteractionAbilityToGrant);

			if (InteractionAbilitySpec)
			{
				Option.TargetASC = AbilitySystemComponent.Get();
				Option.TargetInteractionAbilityHandle = InteractionAbilitySpec->Handle;
			}
		}

		if (InteractionAbilitySpec)
		{
			if (InteractionAbilitySpec->Ability->CanActivateAbility(InteractionAbilitySpec->Handle, AbilitySystemComponent->AbilityActorInfo.Get()))
			{
				NewOptions.Add(Option); //CurrentOptions에 넣어야 함
			}
		}
	}
}