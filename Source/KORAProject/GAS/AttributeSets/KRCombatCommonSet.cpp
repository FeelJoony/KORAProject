#include "GAS/AttributeSets/KRCombatCommonSet.h"

UKRCombatCommonSet::UKRCombatCommonSet()
{
	InitCurrentHealth(100.f);
	InitMaxHealth(100.f);
	InitAttackPower(1.f);
	InitDefensePower(0.f);
	InitDealDamageMult(1.f);
	InitTakeDamageMult(1.f);
	InitDamageTaken(0.f);
}
