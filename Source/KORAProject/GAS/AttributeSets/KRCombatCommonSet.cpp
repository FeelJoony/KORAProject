#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayTag/KREventTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/KRStateTag.h"

UKRCombatCommonSet::UKRCombatCommonSet()
{
	InitCurrentHealth(100.f);
	InitMaxHealth(100.f);
	InitAttackPower(1.f);
	InitDefensePower(0.f);
	InitDealDamageMult(1.f);
	InitTakeDamageMult(1.f);
	InitDamageTaken(0.f);
	InitAttackSpeed(1.f);
	InitCritChance(0.f);
	InitCritMulti(1.5f);
	InitWeaponRange(200.f);
	InitCurrentAmmo(0.f);
	InitMaxAmmo(0.f);
}

void UKRCombatCommonSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetCurrentHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetCurrentAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxAmmo());
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
			const float OldHealth = GetCurrentHealth();
			const float NewHealth = FMath::Clamp(OldHealth - LocalDamageDone, 0.f, GetMaxHealth());
			SetCurrentHealth(NewHealth);

			// 사망 이벤트만 여기서 처리
			// HITREACTION 이벤트는 GEExecCalc_DamageTaken에서 가드 상태를 고려하여 전송
			const bool bIsDead = GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(KRTAG_STATE_ACTING_DEAD);

			if (!bIsDead && NewHealth <= 0.f)
			{
				FGameplayEventData Payload;
				Payload.EventTag = KRTAG_EVENT_COMBAT_DEATH;
				Payload.EventMagnitude = LocalDamageDone;
				Payload.Instigator = Data.EffectSpec.GetEffectContext().GetOriginalInstigator();
				Payload.Target = GetOwningActor();
				Payload.ContextHandle = Data.EffectSpec.GetContext();

				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					GetOwningActor(),
					KRTAG_EVENT_COMBAT_DEATH,
					Payload
				);
			}
		}
	}
}
