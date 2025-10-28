#include "GAS/KRPlayerAbilitySystemComponent.h"

void UKRPlayerAbilitySystemComponent::ApplyInitialEffects()
{
	Super::ApplyInitialEffects();
}

void UKRPlayerAbilitySystemComponent::GiveInitialAbilities()
{
	Super::GiveInitialAbilities();
}

void UKRPlayerAbilitySystemComponent::GiveInputAbilities()
{
	for (const auto& InputAbility : InitialInputAbilities)
	{
		FGameplayAbilitySpec InputSpec(InputAbility.Value);
		InputSpec.InputID = InputAbility.Key;
		GiveAbility(InputSpec);
	}
}
