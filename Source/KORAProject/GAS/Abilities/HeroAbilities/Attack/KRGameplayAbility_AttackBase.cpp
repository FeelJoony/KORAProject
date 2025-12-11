#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_AttackBase.h"

#include "KORADebugHelper.h"
#include "VectorTypes.h"

UKRGameplayAbility_AttackBase::UKRGameplayAbility_AttackBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CurrentComboIndex = 0;
	MaxComboCount = 3;
	ComboResetTime = 0.5f;
}

void UKRGameplayAbility_AttackBase::IncrementCombo()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ComboResetTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ComboResetTimerHandle);
	}

	CurrentComboIndex++;

	if (CurrentComboIndex >= MaxComboCount)
	{
		CurrentComboIndex = 0;
	}

	Debug::Print("[AttackBase] Combo Incremented: " + FString::FromInt(CurrentComboIndex), FColor::Green);
}

void UKRGameplayAbility_AttackBase::ResetCombo()
{
	CurrentComboIndex = 0;
}

void UKRGameplayAbility_AttackBase::CheckComboTimeout()
{
	if (GetWorld()->GetTimeSeconds() - LastAttackEndTime > ComboResetTime)
	{
		ResetCombo();
	}
}

void UKRGameplayAbility_AttackBase::StartComboResetTimer()
{
	LastAttackEndTime = GetWorld()->GetTimeSeconds();
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ComboResetTimerHandle,
			this,
			&ThisClass::OnComboResetTimer,
			ComboResetTime,
			false
			);
	}
}

void UKRGameplayAbility_AttackBase::OnComboResetTimer()
{
	ResetCombo();
}

bool UKRGameplayAbility_AttackBase::IsLastCombo() const
{
	return CurrentComboIndex >= MaxComboCount - 1;
}
