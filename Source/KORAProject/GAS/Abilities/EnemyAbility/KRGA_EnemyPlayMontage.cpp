#include "KRGA_EnemyPlayMontage.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/StateTreeAIComponent.h"

UKRGA_EnemyPlayMontage::UKRGA_EnemyPlayMontage(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	PlayRates.Empty();
	AppliedEffects.Empty();
	CurrentSectionIndex = 0;

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKRGA_EnemyPlayMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		return;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

	CurrentSectionIndex = 0;
	NumSections = SectionNames.Num();
	
	if (MontageToPlay != nullptr && AnimInstance != nullptr && AnimInstance->GetActiveMontageInstance() == nullptr)
	{
		TArray<const UGameplayEffect*> Effects;
		GetGameplayEffectsWhileAnimating(Effects);
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

		if (SectionNames.IsEmpty())
		{
			SectionNames.Add(NAME_None);
		}

		PlayNextSection();
		
		//float const Duration = AnimInstance->Montage_Play(MontageToPlay, PlayRates[CurrentSectionIndex]);

		// FOnMontageEnded EndDelegate;
		// EndDelegate.BindUObject(this, &UKRGA_EnemyPlayMontage::OnMontageEnded, ActorInfo->AbilitySystemComponent, AppliedEffects);
		// AnimInstance->Montage_SetEndDelegate(EndDelegate);
	}

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
}

void UKRGA_EnemyPlayMontage::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
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
	
	OnMontageEnded(MontageToPlay, bWasCancelled, ActorInfo->AbilitySystemComponent);
}

void UKRGA_EnemyPlayMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		for (FActiveGameplayEffectHandle& Effect : AppliedEffects)
		{
			ASC->RemoveActiveGameplayEffect(Effect);
		}
	}
}

void UKRGA_EnemyPlayMontage::GetGameplayEffectsWhileAnimating(TArray<const UGameplayEffect*>& OutEffects) const
{
	for (TSubclassOf<UGameplayEffect> EffectClass : GameplayEffectClassesWhileAnimating)
	{
		if (EffectClass)
		{
			OutEffects.Add(EffectClass->GetDefaultObject<UGameplayEffect>());
		}
	}
}

void UKRGA_EnemyPlayMontage::PlayNextSection()
{
	if (!SectionNames.IsValidIndex(CurrentSectionIndex))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	float PlayRate = PlayRates.IsEmpty() ? 1.f : PlayRates[CurrentSectionIndex];
	FName SectionName = SectionNames[CurrentSectionIndex];

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		MontageToPlay,
		PlayRate,
		SectionName
	);

	Task->OnCompleted.AddDynamic(this, &UKRGA_EnemyPlayMontage::OnSelectionCompleted);
	Task->OnInterrupted.AddDynamic(this, &UKRGA_EnemyPlayMontage::K2_EndAbility);
	Task->ReadyForActivation();
}

void UKRGA_EnemyPlayMontage::OnSelectionCompleted()
{
	CurrentSectionIndex++;
	PlayNextSection();
}
