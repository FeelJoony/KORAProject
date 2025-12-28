#include "GAS/AttributeSets/KRPlayerAttributeSet.h"

UKRPlayerAttributeSet::UKRPlayerAttributeSet()
{
	InitCurrentStamina(100.f);
	InitMaxStamina(100.f);
	InitStaminaRegenRate(30.f);
	InitStaminaRegenDelay(0.5f);
	InitGreyHP(0.f);
	InitCoreDrive(0.f);
	InitMaxCoreDrive(60.f);
}