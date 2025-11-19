#include "AbilityTask_GrantNearbyInteraction.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Interaction/InteractableTarget.h"
#include "Interaction/InteractionOption.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/OverlapResult.h"
#include "GAS/Abilities/HeroAbilities/KRGA_Interact.h"

UAbilityTask_GrantNearbyInteraction* UAbilityTask_GrantNearbyInteraction::GrantAbilitiesForNearbyInteractors(
	UGameplayAbility* OwningAbility,
	bool ShowDebugInfo,
	float InteractionScanRange,
	float InteractionScanRate)
{
	UAbilityTask_GrantNearbyInteraction* MyObj = NewAbilityTask<UAbilityTask_GrantNearbyInteraction>(OwningAbility);
	MyObj->ShowDebugInfo = ShowDebugInfo;
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
		FName(TEXT("Interaction_Sphere")),
		FCollisionShape::MakeSphere(InteractionScanRange),
		QueryParams
	);

	if (ShowDebugInfo)
	{
		DrawDebugSphere(World,AvatarActor->GetActorLocation(),InteractionScanRange,16, FColor::Green, false, InteractionScanRate);
	}
	
	// if (OverlapResults.Num() <= 0)
	// {
	// 	//ClearAllCachedAbility()로 변경
	// 	return;
	// }

	TArray<TScriptInterface<IInteractableTarget>> InteractableTargets;
	
	for (const FOverlapResult& Overlap : OverlapResults)
	{
		//UE_LOG(LogTemp,Warning,TEXT("[Interaction_Sphere] Overlap : %d"),Overlap.ItemIndex);
		
		TScriptInterface<IInteractableTarget> InteractableActor(Overlap.GetActor());
		if (InteractableActor)
		{
			InteractableTargets.AddUnique(InteractableActor);
		}

		TScriptInterface<IInteractableTarget> InteractableComponent(Overlap.GetComponent());
		if (InteractableComponent)
		{
			InteractableTargets.AddUnique(InteractableComponent);
		}
	}

	FInteractionQuery InteractionQuery;
	InteractionQuery.RequestingActor = AvatarActor;
	InteractionQuery.RequestingController=Cast<AController>(AvatarActor->GetOwner());
	
	TArray<FInteractionOption> Options;
	for (TScriptInterface<IInteractableTarget>& InteractableTarget : InteractableTargets)
	{
		FInteractionOptionBuilder InteractionBuilder(InteractableTarget, Options);
		InteractableTarget->GatherInteractionOptions(InteractionQuery, InteractionBuilder);
	}
	UpdateAllTargetOptions(Options);
}

void UAbilityTask_GrantNearbyInteraction::UpdateAllTargetOptions(TArray<FInteractionOption>& Options)
{
	TArray<FInteractionOption> NewOptions;
	TMap<FObjectKey, FGameplayAbilitySpecHandle> AbilitiesToKeep;
	
	//TargetASC가 있는지 확인 후 Option에 대입
	for (FInteractionOption& Option : Options)
	{
		FGameplayAbilitySpec* InteractionAbilitySpec = nullptr;
		if (Option.TargetASC && Option.TargetInteractionAbilityHandle.IsValid())
		{
			InteractionAbilitySpec = Option.TargetASC->FindAbilitySpecFromHandle(Option.TargetInteractionAbilityHandle);
		}
		else if (Option.InteractionAbilityToGrant) //if로 변경 고려
		{
			InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Option.InteractionAbilityToGrant);

			if (!InteractionAbilitySpec)
			{
				AActor* AvatarActor = AbilitySystemComponent->GetAvatarActor();
				FGameplayAbilitySpec Spec(Option.InteractionAbilityToGrant, 1, INDEX_NONE, AvatarActor);
				FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(Spec);
				InteractionAbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
			}
			
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
				NewOptions.Emplace(Option);

				if (Option.InteractionAbilityToGrant)
				{
					FObjectKey ObjectKey(Option.InteractionAbilityToGrant);
					AbilitiesToKeep.Add(ObjectKey, InteractionAbilitySpec->Handle);
				}
			}
		}
	}

	bool bOptionChanged = false;
	//UE_LOG(LogTemp, Warning, TEXT("NewOptions Num: %d, CurrentOptions Num: %d"), NewOptions.Num(), CurrentOptions.Num());
	if (NewOptions.Num() != CurrentOptions.Num())
	{
		bOptionChanged = true;
	}
	else
	{
		for (int32 i = 0; i < NewOptions.Num(); ++i)
		{
			if (NewOptions[i] != CurrentOptions[i])
			{
				bOptionChanged = true;
				break;
			}
		}
	}

	if (bOptionChanged)
	{
		CleanOutRangeAbility(AbilitiesToKeep);
		InteractionAbilityCache=AbilitiesToKeep;
		
		SortOptions(NewOptions);
		CurrentOptions=NewOptions;

		if (Ability)
		{
			if (UKRGA_Interact* InteractAbility = Cast<UKRGA_Interact>(Ability))
			{
				InteractAbility->UpdateInteractions(CurrentOptions);
			}
		}
		
		InteractionChanged.Broadcast(CurrentOptions);//옵션을 매개변수로 이벤트 실행 (ex : IndicatorComponent에서 Option[0]을 UI에 표시
	}
}

void UAbilityTask_GrantNearbyInteraction::CleanOutRangeAbility(const TMap<FObjectKey, FGameplayAbilitySpecHandle>& AbilitiesToKeep)
{
	if (!AbilitySystemComponent.IsValid())
	{
		return;
	}

	for (auto It = InteractionAbilityCache.CreateIterator(); It; ++It)
	{
		if (!AbilitiesToKeep.Contains(It.Key()))
		{
			FGameplayAbilitySpecHandle HandleToRemove = It.Value();
			AbilitySystemComponent->ClearAbility(HandleToRemove);
            
			It.RemoveCurrent();
		}
	}
}

void UAbilityTask_GrantNearbyInteraction::SortOptions(TArray<FInteractionOption>& Options)
{
	Options.Sort([&](const FInteractionOption& A, const FInteractionOption& B)
		{
			if (A.Priority != B.Priority)
			{
				return A.Priority > B.Priority;
			}
			return CompareDistance(A) < CompareDistance(B);
		});
}

float UAbilityTask_GrantNearbyInteraction::CompareDistance(const FInteractionOption& Option) const
{
	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UAbilityTask_GrantNearbyInteraction] AvatarActor is NULL"))
		return FLT_MAX;
	}
	
	const FVector PlayerLocation = AvatarActor->GetActorLocation();

	if (UObject* TargetObject = Option.InteractableTarget.GetObject())
	{
		FVector TargetLocation;
		if (const USceneComponent* TargetSceneComponent = Cast<USceneComponent>(TargetObject))
		{
			TargetLocation = TargetSceneComponent->GetComponentLocation(); 
		}
		else if (const AActor* TargetActor = Cast<AActor>(TargetObject))
		{
			TargetLocation = TargetActor->GetActorLocation();
		}
		else if (const UActorComponent* TargetComponent = Cast<UActorComponent>(TargetObject))
		{
			if (TargetComponent->GetOwner())
			{
				TargetLocation = TargetComponent->GetOwner()->GetActorLocation();
			}
		}
		return FVector::DistSquared(PlayerLocation, TargetLocation);
	}
	return FLT_MAX;
}

