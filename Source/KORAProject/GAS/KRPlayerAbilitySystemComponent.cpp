#include "GAS/KRPlayerAbilitySystemComponent.h"
#include "Abilities/KRHeroGameplayAbility.h"
#include "AI/NavigationModifier.h"

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

	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.Ability) continue;

		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
		{
			AbilitiesToActivate.Add(AbilitySpec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		TryActivateAbility(Handle);
	}
}

void UKRPlayerAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}

void UKRPlayerAbilitySystemComponent::GrantHeroWeaponAbilities(
	const TArray<FKRHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel,
	TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
	if (InDefaultWeaponAbilities.IsEmpty()) return;

	for (const FKRHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
	{
		if (!AbilitySet.IsValid()) continue;
		
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);

		OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UKRPlayerAbilitySystemComponent::RemovedGrantedHeroWeaponAbilities(
	TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty()) return;

	for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
	{
		if (SpecHandle.IsValid())
		{
			ClearAbility(SpecHandle);
		}
	}
	InSpecHandlesToRemove.Empty();
}
