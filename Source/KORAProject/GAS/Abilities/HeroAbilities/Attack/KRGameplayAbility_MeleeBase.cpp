#include "KRGameplayAbility_MeleeBase.h"
#include "Characters/KRBaseCharacter.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "AbilitySystemBlueprintLibrary.h"

UKRGameplayAbility_MeleeBase::UKRGameplayAbility_MeleeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKRGameplayAbility_MeleeBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ListenForHitEvent();
}

void UKRGameplayAbility_MeleeBase::ListenForHitEvent()
{
	FGameplayTag HitTag = KRTAG_EVENT_COMBAT_HIT;

	UAbilityTask_WaitGameplayEvent* Task = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitTag,
		nullptr,
		false,
		false
		);

	if (Task)
	{
		Task->EventReceived.AddDynamic(this, &ThisClass::OnTargetHit);
		Task->ReadyForActivation();
	}
}

void UKRGameplayAbility_MeleeBase::OnTargetHit(FGameplayEventData Payload)
{
	AActor* TargetActor = const_cast<AActor*>(Payload.Target.Get());
	if (!TargetActor) return;

	float WeaponBaseDamage = Payload.EventMagnitude;
	float FinalDamage = WeaponBaseDamage * CurrentDamageMultiplier;

	if (DamageEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(
				KRTAG_SETBYCALLER_BASEDAMAGE,
				FinalDamage
				);

			if (Payload.ContextHandle.GetHitResult())
			{
				SpecHandle.Data->GetContext().AddHitResult(*Payload.ContextHandle.GetHitResult());
			}

			FGameplayAbilityTargetDataHandle TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(TargetActor);

			TArray<FActiveGameplayEffectHandle> AppliedHandles = ApplyGameplayEffectSpecToTarget(
				CurrentSpecHandle,
				CurrentActorInfo,
				CurrentActivationInfo,
				SpecHandle,
				TargetData
				);
		}
	}
}
