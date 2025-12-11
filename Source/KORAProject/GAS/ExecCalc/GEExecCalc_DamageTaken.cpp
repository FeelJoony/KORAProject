#include "GAS/ExecCalc/GEExecCalc_DamageTaken.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KREnemyAttributeSet.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRSetByCallerTag.h"

static bool IsHitFromFront(const FGameplayEffectSpec& Spec, UAbilitySystemComponent* TargetASC)
{
	const FHitResult* Hit = Spec.GetContext().GetHitResult();
	if (!Hit || !TargetASC)
	{
		return true;
	}

	const AActor* TargetActor = TargetASC->GetAvatarActor();
	const AActor* InstigatorActor = Spec.GetContext().GetOriginalInstigator();

	if (!TargetActor || !InstigatorActor)
	{
		return true;
	}

	const FVector TargetForward = TargetActor->GetActorForwardVector();
	const FVector FromAttacker = (TargetActor->GetActorLocation() - InstigatorActor->GetActorLocation()).GetSafeNormal();
	const float Dot = FVector::DotProduct(TargetForward, FromAttacker);

	return Dot >= 0.f;
}


UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	if (!TargetASC)
	{
		return;
	}

	float RawDamage = Spec.GetSetByCallerMagnitude(KRTAG_SETBYCALLER_BASEDAMAGE, false, 0.f);
	if (RawDamage <= 0.f)
	{
		return;
	}

}
