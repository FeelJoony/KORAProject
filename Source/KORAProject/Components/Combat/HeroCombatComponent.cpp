#include "Components/Combat/HeroCombatComponent.h"
#include "Items/Weapons/KRHeroMeleeWeapon.h"

AKRHeroMeleeWeapon* UHeroCombatComponent::GetHeroCarriedMeleeWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<AKRHeroMeleeWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}
