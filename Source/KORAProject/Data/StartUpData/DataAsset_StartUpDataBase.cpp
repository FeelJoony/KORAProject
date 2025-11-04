#include "Data/StartUpData/DataAsset_StartUpDataBase.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/Abilities/KRGameplayAbility.h"

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UKRAbilitySystemComponent* InASC, int32 ApplyLevel)
{
	check(InASC);

	GrantAbilities(ActivateOnGivenAbilities, InASC, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASC, ApplyLevel);
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UKRGameplayAbility>>& InAbilitiesToGive,
	UKRAbilitySystemComponent* InASC, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty()) return;

	for (const TSubclassOf<UKRGameplayAbility>& Ability : InAbilitiesToGive)
	{
		if (!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASC->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		InASC->GiveAbility(AbilitySpec);
	}
}
