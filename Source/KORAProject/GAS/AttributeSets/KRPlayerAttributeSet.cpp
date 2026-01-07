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
	InitCurrentAmmo(0.f);
	InitMaxAmmo(0.f);
}

void UKRPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCurrentAmmoAttribute())
	{
		float MaxVal = GetMaxAmmo();
		UE_LOG(LogTemp, Warning, TEXT(">> [AmmoCheck] New: %f, Max: %f"), NewValue, MaxVal);

		NewValue = FMath::Clamp(NewValue, 0.f, MaxVal);
	}
}