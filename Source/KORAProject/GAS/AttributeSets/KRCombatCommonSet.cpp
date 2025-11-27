#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayEffectExtension.h"

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

void UKRCombatCommonSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UKRCombatCommonSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float LocalDamageDone = GetDamageTaken();
		SetDamageTaken(0.f);

		if (LocalDamageDone > 0.f)
		{
			const float NewHealth = GetCurrentHealth() - LocalDamageDone;
			SetCurrentHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			// 사망처리 혹은 메세지 시스템을 활용해 이벤트 전송
		}
	}
}
