#include "KORATypes/KRStructTypes.h"
#include "GAS/Abilities/KRHeroGameplayAbility.h"

bool FKRHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
