#include "GAS/AttributeSets/KRPlayerAttributeSet.h"

UKRPlayerAttributeSet::UKRPlayerAttributeSet()
{
	InitCurrentStamina(1.f);
	InitMaxStamina(1.f);
	InitGreyHP(0.f);
	InitStaminaRegenRate(1.f);
	InitStaminaRegenDelay(1.f);
	InitCoreDrive(0.f);
	InitMaxCoreDrive(60.f);
}