#include "KORATypes/KRStructTypes.h"
#include "GAS/Abilities/KRGameplayAbility.h"

bool FKRHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
