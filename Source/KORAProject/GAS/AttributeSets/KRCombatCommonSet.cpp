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
			const float OldHealth = GetCurrentHealth();
			const float NewHealth = FMath::Clamp(OldHealth - LocalDamageDone, 0.f, GetMaxHealth());
			SetCurrentHealth(NewHealth);

			FGameplayEventData Payload;
			Payload.EventTag = (NewHealth <= 0.f) ? KRTAG_EVENT_COMBAT_DEATH : KRTAG_EVENT_COMBAT_HIT;
			Payload.EventMagnitude = LocalDamageDone;

			Payload.Instigator = Data.EffectSpec.GetEffectContext().GetOriginalInstigator();
			Payload.Target = GetOwningActor();
			Payload.ContextHandle = Data.EffectSpec.GetContext();

			const bool bIsDead = GetOwningAbilitySystemComponent()->HasMatchingGameplayTag(KRTAG_STATE_ACTING_DEAD);

			if (!bIsDead)
			{
				if (NewHealth <= 0.f)
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						GetOwningActor(),
						KRTAG_EVENT_COMBAT_DEATH,
						Payload
						);
				}
				else
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						GetOwningActor(),
						KRTAG_EVENT_COMBAT_HITREACTION,
						Payload
						);
				}
			}
		}
	}
}
