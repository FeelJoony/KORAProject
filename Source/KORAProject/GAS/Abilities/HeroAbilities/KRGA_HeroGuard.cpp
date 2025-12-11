#include "GAS/Abilities/HeroAbilities/KRGA_HeroGuard.h"


UKRGA_HeroGuard::UKRGA_HeroGuard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKRGA_HeroGuard::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UKRGA_HeroGuard::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HeroGuard::OnGuardInputReleased(float TimeHeld)
{
}

void UKRGA_HeroGuard::OnGuardBrokenEventReceived(FGameplayEventData Payload)
{
}

void UKRGA_HeroGuard::OnParrySuccessEventReceived(FGameplayEventData Payload)
{
}

void UKRGA_HeroGuard::StartGuardState()
{
}

void UKRGA_HeroGuard::StopGuardState()
{
}
