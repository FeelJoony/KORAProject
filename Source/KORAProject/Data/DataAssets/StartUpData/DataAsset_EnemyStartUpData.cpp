#include "Data/DataAssets//StartUpData/DataAsset_EnemyStartUpData.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/Abilities/KREnemyGameplayAbility.h"

void UDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(UKRAbilitySystemComponent* InASC, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASC, ApplyLevel);

	if (!EnemyCombatAbilities.IsEmpty())
	{
		for (const TSubclassOf<UKREnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities)
		{
			if (!AbilityClass) continue;

			FGameplayAbilitySpec AbilitySpec(AbilityClass);
			AbilitySpec.SourceObject = InASC->GetAvatarActor();
			AbilitySpec.Level = ApplyLevel;

			InASC->GiveAbility(AbilitySpec);
		}
	}
}
