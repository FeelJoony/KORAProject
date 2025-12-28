#include "GAS/ExecCalc/GEExecCalc_DamageTaken.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRSetByCallerTag.h"

// ─────────────────────────────────────────────────────────────────────────────
// Capture할 속성 정의
// ─────────────────────────────────────────────────────────────────────────────
struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DealDamageMult);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CritMulti);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(TakeDamageMult);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken);
	DECLARE_ATTRIBUTE_CAPTUREDEF(GreyHP);

	FDamageStatics()
	{
		// Source (공격자) 속성
		DEFINE_ATTRIBUTE_CAPTUREDEF(UKRCombatCommonSet, DealDamageMult, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UKRCombatCommonSet, CritChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UKRCombatCommonSet, CritMulti, Source, false);

		// Target (피격자) 속성
		DEFINE_ATTRIBUTE_CAPTUREDEF(UKRCombatCommonSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UKRCombatCommonSet, TakeDamageMult, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UKRCombatCommonSet, DamageTaken, Target, false);

		// Target GreyHP (가드 리게인용, PlayerAttributeSet)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UKRPlayerAttributeSet, GreyHP, Target, false);
	}
};

static const FDamageStatics& DamageStatics()
{
	static FDamageStatics DmgStatics;
	return DmgStatics;
}

// ─────────────────────────────────────────────────────────────────────────────
// 생성자
// ─────────────────────────────────────────────────────────────────────────────
UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	// Capture할 속성 등록
	RelevantAttributesToCapture.Add(DamageStatics().DealDamageMultDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CritMultiDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefensePowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().TakeDamageMultDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageTakenDef);
	RelevantAttributesToCapture.Add(DamageStatics().GreyHPDef);
}

// ─────────────────────────────────────────────────────────────────────────────
// Execute_Implementation
// ─────────────────────────────────────────────────────────────────────────────
void UGEExecCalc_DamageTaken::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	if (!TargetASC)
	{
		return;
	}

	// ─────────────────────────────────────────────────────
	// 1. 기본 데미지 가져오기 (GA에서 SetByCaller로 전달)
	// ─────────────────────────────────────────────────────
	float BaseDamage = Spec.GetSetByCallerMagnitude(KRTAG_SETBYCALLER_BASEDAMAGE, false, 0.f);
	if (BaseDamage <= 0.f)
	{
		return;
	}

	// ─────────────────────────────────────────────────────
	// 2. 공격자 속성 Capture
	// ─────────────────────────────────────────────────────
	float SourceDealDamageMult = 1.0f;
	float SourceCritChance = 0.0f;
	float SourceCritMulti = 1.5f;

	const FAggregatorEvaluateParameters EvalParams;

	if (SourceASC)
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().DealDamageMultDef, EvalParams, SourceDealDamageMult);
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().CritChanceDef, EvalParams, SourceCritChance);
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			DamageStatics().CritMultiDef, EvalParams, SourceCritMulti);
	}

	// ─────────────────────────────────────────────────────
	// 3. 피격자 속성 Capture
	// ─────────────────────────────────────────────────────
	float TargetDefensePower = 0.0f;
	float TargetTakeDamageMult = 1.0f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().DefensePowerDef, EvalParams, TargetDefensePower);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().TakeDamageMultDef, EvalParams, TargetTakeDamageMult);

	// ─────────────────────────────────────────────────────
	// 4. 크리티컬 계산
	// ─────────────────────────────────────────────────────
	float CritMultiplier = 1.0f;
	bool bIsCritical = false;

	if (FMath::FRand() < SourceCritChance)
	{
		CritMultiplier = SourceCritMulti;
		bIsCritical = true;
	}

	// ─────────────────────────────────────────────────────
	// 5. 가드/패리 상태 체크
	// ─────────────────────────────────────────────────────
	float GuardDamageReduction = 0.0f;
	bool bIsGuarding = false;
	bool bIsParried = false;

	// 가드 상태 체크
	if (TargetASC->HasMatchingGameplayTag(KRTAG_STATE_ACTING_GUARD))
	{
		bIsGuarding = true;

		// 전방 공격인지 확인
		const bool bHitFromFront = IsHitFromFront(Spec, TargetASC);

		if (bHitFromFront)
		{
			// 패리 타이밍 체크 (패리 상태 태그가 있으면 패리 성공)
			if (TargetASC->HasMatchingGameplayTag(KRTAG_STATE_ACTING_PARRY))
			{
				bIsParried = true;
				GuardDamageReduction = 1.0f;  // 패리 성공: 데미지 완전 무효화

				// 패리 성공 이벤트 전송
				FGameplayEventData ParryEvent;
				ParryEvent.EventTag = KRTAG_EVENT_COMBAT_PARRYSUCCESS;
				ParryEvent.Instigator = Spec.GetContext().GetOriginalInstigator();
				ParryEvent.Target = TargetASC->GetAvatarActor();

				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					TargetASC->GetAvatarActor(),
					KRTAG_EVENT_COMBAT_PARRYSUCCESS,
					ParryEvent
				);

				// 공격자에게도 패리 당함 이벤트 전송 (스태거 등 처리용)
				if (SourceASC)
				{
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						SourceASC->GetAvatarActor(),
						KRTAG_EVENT_COMBAT_PARRYSUCCESS,
						ParryEvent
					);
				}
			}
			else
			{
				// 일반 가드: 70% 데미지 감소
				GuardDamageReduction = 0.7f;
			}
		}
		else
		{
			// 후방 공격: 가드 무시
			GuardDamageReduction = 0.0f;
		}
	}

	// ─────────────────────────────────────────────────────
	// 6. 최종 데미지 계산
	// ─────────────────────────────────────────────────────
	// 공식: (BaseDamage × DealDamageMult × CritMult - Defense) × TakeDamageMult × (1 - GuardReduction)
	float FinalDamage = BaseDamage * SourceDealDamageMult * CritMultiplier;
	FinalDamage = FMath::Max(0.f, FinalDamage - TargetDefensePower);
	FinalDamage *= TargetTakeDamageMult;
	FinalDamage *= (1.0f - GuardDamageReduction);

	// 최소 데미지 보장 (가드/패리가 아닌 경우)
	if (!bIsGuarding && FinalDamage < 1.0f && BaseDamage > 0.f)
	{
		FinalDamage = 1.0f;
	}

	// ─────────────────────────────────────────────────────
	// 7. DamageTaken에 출력
	// ─────────────────────────────────────────────────────
	if (FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				DamageStatics().DamageTakenProperty,
				EGameplayModOp::Additive,
				FinalDamage
			)
		);
	}

	// ─────────────────────────────────────────────────────
	// 8. 일반 가드 시 GreyHP 생성 (가드 리게인)
	// ─────────────────────────────────────────────────────
	// 일반 가드로 막은 경우 (패리 X, 전방 피격 O), 받은 데미지만큼 GreyHP 생성
	// 이 GreyHP는 플레이어가 공격하면 실제 HP로 회복 가능
	const bool bIsStandardGuard = bIsGuarding && !bIsParried && (GuardDamageReduction > 0.f);
	if (bIsStandardGuard && FinalDamage > 0.f)
	{
		// GreyHP 생성량 = 받은 실제 데미지
		const float GreyHPToAdd = FinalDamage;

		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				DamageStatics().GreyHPProperty,
				EGameplayModOp::Additive,
				GreyHPToAdd
			)
		);

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[GreyHP] Standard guard hit - added %.1f GreyHP"), GreyHPToAdd);
#endif
	}

#if !UE_BUILD_SHIPPING
	// 디버그 로그
	UE_LOG(LogTemp, Log, TEXT("[DamageTaken] BaseDamage: %.1f, DealMult: %.2f, CritMult: %.2f, Defense: %.1f, TakeMult: %.2f, GuardRed: %.2f -> Final: %.1f %s%s"),
		BaseDamage, SourceDealDamageMult, CritMultiplier, TargetDefensePower, TargetTakeDamageMult, GuardDamageReduction, FinalDamage,
		bIsCritical ? TEXT("[CRIT]") : TEXT(""),
		bIsParried ? TEXT("[PARRIED]") : (bIsGuarding ? TEXT("[GUARDED]") : TEXT("")));
#endif
}


bool UGEExecCalc_DamageTaken::IsHitFromFront(const FGameplayEffectSpec& Spec, UAbilitySystemComponent* TargetASC) const
{
	if (!TargetASC)
	{
		return true;
	}

	const AActor* TargetActor = TargetASC->GetAvatarActor();
	const AActor* InstigatorActor = Spec.GetContext().GetOriginalInstigator();

	if (!TargetActor || !InstigatorActor)
	{
		return true;  // 정보 없으면 전방으로 가정
	}

	const FVector TargetForward = TargetActor->GetActorForwardVector();
	const FVector ToAttacker = (InstigatorActor->GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal();
	const float Dot = FVector::DotProduct(TargetForward, ToAttacker);

	// Dot > 0: 공격자가 피격자 앞에 있음 (전방 공격)
	return Dot > 0.f;
}
