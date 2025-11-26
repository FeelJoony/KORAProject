#include "KRGA_Interact.h"

#include "AbilitySystemComponent.h"

void UKRGA_Interact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UKRGA_Interact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_Interact::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	// ASC 가져오기
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	
	// GameplayAbilityMap에서 Has Tag 찾아서 GA 있으면 발동하기
	for (FGameplayTag& Tag : GameplayAbilityTags)
	{
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			FGameplayTagContainer TagContainer(Tag);
			if (ASC->TryActivateAbilitiesByTag(TagContainer))
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
			}
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UKRGA_Interact::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
