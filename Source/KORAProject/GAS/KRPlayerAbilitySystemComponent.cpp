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

void UKRPlayerAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid()) return;

	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag)) continue;

		TryActivateAbility(AbilitySpec.Handle);
		
	}
}

void UKRPlayerAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}
