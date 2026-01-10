#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_EnemyAttack.h"

#include "AbilitySystemComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Characters/KRHeroCharacter.h"

void UKRGameplayAbility_EnemyAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
	AppliedEffects.Empty();
	BeginOnlyTriggerOnce=false;

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (MontageToPlay != nullptr && AnimInstance != nullptr && AnimInstance->GetActiveMontageInstance() == nullptr)
	{
		TArray<const UGameplayEffect*> Effects;
		
		for (TSubclassOf<UGameplayEffect> EffectClass : GameplayEffectClassesWhileAnimating)
		{
			if (EffectClass)
			{
				Effects.Add(EffectClass->GetDefaultObject<UGameplayEffect>());
			}
		}

		if (Effects.Num() > 0)
		{
			UAbilitySystemComponent* const AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
			for (const UGameplayEffect* Effect : Effects)
			{
				FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect, 1.f, MakeEffectContext(Handle, ActorInfo));
				if (EffectHandle.IsValid())
				{
					AppliedEffects.Add(EffectHandle);
				}
			}
		}
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo->OwnerActor.IsValid())
	{
		if (AActor* OwnerActor = ActorInfo->OwnerActor.Get())
		{
			if (UStateTreeAIComponent* StateTreeAIComp = OwnerActor->GetComponentByClass<UStateTreeAIComponent>())
			{
				if (AbilityTags.Num() <= 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Montage AbilityTags is Empty"));
				}

				StateTreeAIComp->SendStateTreeEvent(AbilityTags.GetByIndex(0));
			}
		}
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Active Ability"));
}

void UKRGameplayAbility_EnemyAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo->OwnerActor.IsValid())
	{
		if (AActor* OwnerActor = ActorInfo->OwnerActor.Get())
		{
			if (UStateTreeAIComponent* StateTreeAIComp = OwnerActor->GetComponentByClass<UStateTreeAIComponent>())
			{
				if (EndAbilityTags.Num() <= 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("Montage AbilityTags is Empty"));
				}

				StateTreeAIComp->SendStateTreeEvent(EndAbilityTags.GetByIndex(0));
			}
		}
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();;
	if (ASC)
	{
		for (FActiveGameplayEffectHandle& Effect : AppliedEffects)
		{
			ASC->RemoveActiveGameplayEffect(Effect);
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UAnimMontage* UKRGameplayAbility_EnemyAttack::GetCurrentMontage() const
{
	return MontageToPlay;
}

void UKRGameplayAbility_EnemyAttack::ProcessHitResults(const TArray<FHitResult>& HitResults)
{
	// 플레이어만 필터링
	TArray<FHitResult> FilteredHits;
	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->IsA<AKRHeroCharacter>())
			{
				FilteredHits.Add(Hit);
			}
		}
	}

	Super::ProcessHitResults(FilteredHits);
}

