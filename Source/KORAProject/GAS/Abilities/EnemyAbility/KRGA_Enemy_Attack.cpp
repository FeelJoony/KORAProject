#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Attack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Data/KRMeleeAttackTypes.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "GameFramework/PlayerController.h"

UKRGA_Enemy_Attack::UKRGA_Enemy_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UKRGA_Enemy_Attack::PerformShapeTrace(TArray<FHitResult>& OutHitResults) const
{
	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();

	bool bHit = false;
	switch (Config.AttackShape)
	{
	case EKRAttackShapeType::Cone:
		bHit = PerformConeTrace(OutHitResults);
		break;
	case EKRAttackShapeType::Box:
		bHit = PerformBoxTrace(OutHitResults);
		break;
	case EKRAttackShapeType::Sphere:
		bHit = PerformSphereTrace(OutHitResults);
		break;
	case EKRAttackShapeType::Capsule:
		bHit = PerformCapsuleTrace(OutHitResults);
		break;
	}

	// 디버그 시각화
	if (bDrawDebugHitCheck)
	{
		AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (AvatarActor)
		{
			UKRMeleeAttackLibrary::DrawDebugAttackShape(
				AvatarActor,
				AvatarActor->GetActorLocation(),
				AvatarActor->GetActorForwardVector(),
				Config,
				bHit ? FColor::Green : FColor::Red,
				0.0f  // 한 프레임만 표시
			);
		}
	}

	return bHit;
}

bool UKRGA_Enemy_Attack::PerformConeTrace(TArray<FHitResult>& OutHitResults) const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return false;

	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	const FVector Origin = AvatarActor->GetActorLocation();
	const FVector Forward = AvatarActor->GetActorForwardVector();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	QueryParams.bReturnPhysicalMaterial = false;

	// 원뿔 형태: 여러 Line Trace로 구현
	const int32 NumRays = FMath::Max(5, static_cast<int32>(Config.ConeAngle / 15.0f));
	const float AngleStep = Config.ConeAngle / (NumRays - 1);
	const float StartAngle = -Config.ConeAngle * 0.5f;

	TSet<AActor*> HitActorsSet;

	for (int32 i = 0; i < NumRays; ++i)
	{
		const float CurrentAngle = StartAngle + (AngleStep * i);
		const FVector Direction = Forward.RotateAngleAxis(CurrentAngle, FVector::UpVector);
		const FVector End = Origin + Direction * Config.AttackRange;

		TArray<FHitResult> RayHits;
		if (AvatarActor->GetWorld()->LineTraceMultiByChannel(
			RayHits, Origin, End, ECC_Pawn, QueryParams))
		{
			for (const FHitResult& Hit : RayHits)
			{
				if (Hit.GetActor() && !HitActorsSet.Contains(Hit.GetActor()))
				{
					HitActorsSet.Add(Hit.GetActor());
					OutHitResults.Add(Hit);
				}
			}
		}
	}

	return OutHitResults.Num() > 0;
}

bool UKRGA_Enemy_Attack::PerformBoxTrace(TArray<FHitResult>& OutHitResults) const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return false;

	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	const FVector Origin = AvatarActor->GetActorLocation();
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector End = Origin + Forward * Config.AttackRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	FCollisionShape BoxShape = FCollisionShape::MakeBox(Config.BoxExtent);

	return AvatarActor->GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Origin,
		End,
		AvatarActor->GetActorQuat(),
		ECC_Pawn,
		BoxShape,
		QueryParams
	);
}

bool UKRGA_Enemy_Attack::PerformSphereTrace(TArray<FHitResult>& OutHitResults) const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return false;

	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	const FVector Origin = AvatarActor->GetActorLocation();
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector End = Origin + Forward * Config.AttackRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(Config.SphereRadius);

	return AvatarActor->GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Origin,
		End,
		FQuat::Identity,
		ECC_Pawn,
		SphereShape,
		QueryParams
	);
}

bool UKRGA_Enemy_Attack::PerformCapsuleTrace(TArray<FHitResult>& OutHitResults) const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return false;

	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	const FVector Origin = AvatarActor->GetActorLocation();
	const FVector Forward = AvatarActor->GetActorForwardVector();
	const FVector End = Origin + Forward * Config.AttackRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(
		Config.CapsuleRadius,
		Config.CapsuleHalfHeight
	);

	return AvatarActor->GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Origin,
		End,
		FQuat::Identity,
		ECC_Pawn,
		CapsuleShape,
		QueryParams
	);
}

void UKRGA_Enemy_Attack::PerformHitCheck()
{
	TArray<FHitResult> HitResults;
	if (PerformShapeTrace(HitResults))
	{
		ProcessHitResults(HitResults);
	}
}

void UKRGA_Enemy_Attack::ProcessHitResults(const TArray<FHitResult>& HitResults)
{
	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();

	int32 HitCount = 0;
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor) continue;

		if (HitActorsThisSwing.Contains(HitActor)) continue;

		if (!Config.bCanHitMultiple && HitCount > 0) break;
		if (Config.bCanHitMultiple && HitCount >= Config.MaxHitCount) break;

		ApplyHitToTarget(HitActor, Hit);

		HitActorsThisSwing.Add(HitActor);
		HitCount++;
	}
}

const FKRMeleeAttackConfig& UKRGA_Enemy_Attack::GetCurrentAttackConfig() const
{
	return AttackConfig;
}

void UKRGA_Enemy_Attack::ApplyHitToTarget(AActor* HitActor, const FHitResult& HitResult)
{
	if (!HitActor) return;

	SendHitReactionEvent(HitActor, HitResult);

	ApplyDamage(HitActor, HitResult);

	ExecuteHitCue(HitResult);
}

void UKRGA_Enemy_Attack::ApplyDamage(AActor* TargetActor, const FHitResult& HitResult)
{
	if (!TargetActor || !DamageEffectClass) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!SourceASC || !TargetASC) return;

	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();

	// CombatCommonSet에서 AttackPower 조회
	float AttackPower = 1.0f;
	if (const UKRCombatCommonSet* CombatSet = SourceASC->GetSet<UKRCombatCommonSet>())
	{
		AttackPower = CombatSet->GetAttackPower();
	}

	// 기본 공격력 * 스킬 배율
	float BaseDamage = AttackPower * Config.DamageMultiplier;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_BASEDAMAGE, BaseDamage);
		SpecHandle.Data->GetContext().AddHitResult(HitResult);

		ApplyGameplayEffectSpecToTarget(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			SpecHandle,
			UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(TargetActor)
		);
	}
}

void UKRGA_Enemy_Attack::SendHitReactionEvent(AActor* TargetActor, const FHitResult& HitResult)
{
	if (!TargetActor) return;

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;

	FGameplayEventData EventData;
	EventData.EventTag = KRTAG_EVENT_COMBAT_HITREACTION;
	EventData.Instigator = GetAvatarActorFromActorInfo();
	EventData.Target = TargetActor;
	EventData.ContextHandle = SourceASC->MakeEffectContext();
	EventData.ContextHandle.AddHitResult(HitResult);

	// 히트 강도 + 넉백 거리 패킹하여 전달
	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	float PackedMagnitude = static_cast<float>(Config.HitIntensity) + (Config.KnockbackDistance * 0.001f);
	EventData.EventMagnitude = PackedMagnitude;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		TargetActor,
		KRTAG_EVENT_COMBAT_HITREACTION,
		EventData
	);
}

void UKRGA_Enemy_Attack::ExecuteSwingCue()
{
	if (!SwingCueTag.IsValid()) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	FGameplayCueParameters CueParams;
	CueParams.Location = AvatarActor->GetActorLocation();
	CueParams.Normal = AvatarActor->GetActorForwardVector();

	ASC->ExecuteGameplayCue(SwingCueTag, CueParams);
}

void UKRGA_Enemy_Attack::ExecuteHitCue(const FHitResult& HitResult)
{
	if (!HitCueTag.IsValid()) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	FGameplayCueParameters CueParams;
	CueParams.Location = HitResult.ImpactPoint;
	CueParams.Normal = HitResult.ImpactNormal;

	ASC->ExecuteGameplayCue(HitCueTag, CueParams);
}

void UKRGA_Enemy_Attack::ApplyCameraShake()
{
	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	if (!Config.HitCameraShake) return;

	// 타겟이 플레이어인 경우 플레이어 컨트롤러를 통해 쉐이크 적용
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (PC)
	{
		PC->ClientStartCameraShake(Config.HitCameraShake);
	}
}
