#include "GAS/Abilities/HeroAbilities/KRGA_CoreDriveSurge_Sword.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/KRCoreDriveComponent.h"
#include "Data/DataAssets/KRStarDashData.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRMotionWarpingLibrary.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRStarEffectSpawner.h"
#include "GAS/Abilities/HeroAbilities/KRGA_LockOn.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

UKRGA_CoreDriveSurge_Sword::UKRGA_CoreDriveSurge_Sword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_ATTACK_COREDRIVESURGE);
	SetAssetTags(Tags);

	ActivationPolicy = EKRAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 기본 Cue 태그 설정
	DashStartCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Surge.Start"), false);
	DashHitCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Surge.Hit"), false);

	// 히트 체크 이벤트 태그 (AnimNotifyState에서 발송)
	HitCheckEventTag = KRTAG_EVENT_COREDRIVE_DASH_HITCHECK;
}

void UKRGA_CoreDriveSurge_Sword::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 1. 코어드라이브 소모 체크 및 소모
	UKRCoreDriveComponent* CoreDriveComp = GetCoreDriveComponent();
	if (!CoreDriveComp || !CoreDriveComp->HasEnoughCoreDrive(CoreDriveCost))
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[CoreDriveSurge] Not enough CoreDrive (Required: %.1f)"), CoreDriveCost);
#endif
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 코어드라이브 소모
	CoreDriveComp->ConsumeCoreDrive(CoreDriveCost);

	// 2. 이동 비활성화 (시전 중 이동 불가 + 중력 비활성화)
	DisableMovement();

	// 3. 무적 적용
	ApplyInvincibility();

	// 4. 모션 워핑 설정 (DashStartLocation/DashEndLocation 초기화)
	SetupDashWarping();

	// 5. 디버그 시각화
	DrawDebugHitBox();

	// 4. 콜리전 변경 (적 통과) - MoveIgnoreActors 방식
	// 주의: SetupDashWarping 이후에 호출해야 DashLocation 사용 가능
	SetCollisionIgnorePawns(true);

	// 5. 히트 액터 초기화
	HitActors.Empty();

	// 6. 대시 시작 Cue
	ExecuteDashStartCue();

	// 7. 대시 궤적 이펙트 스폰 (Star Effect)
	SpawnDashTrailEffect();

	// 8. 몽타주 재생
	if (!DashMontage)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Error, TEXT("[CoreDriveSurge] DashMontage is not set!"));
#endif
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		DashMontage,
		1.0f,
		NAME_None,
		false
	);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->ReadyForActivation();
	}

	// 9. GameplayEvent 리스너 설정
	// AnimNotifyState가 GameplayEvent를 발송하면 PerformDashDamageFromNotify() 호출
	SetupHitCheckEventListener();

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Activated - DashDistance: %.1f"), DashDistance);
#endif
}

void UKRGA_CoreDriveSurge_Sword::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 상태 복원
	RemoveInvincibility();
	SetCollisionIgnorePawns(false);

	// 이동 활성화 (중력 복원 포함)
	EnableMovement();

	// 워프 타겟 제거
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		UKRMotionWarpingLibrary::RemoveWarpTarget(AvatarActor, WarpTargetName);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UKRCoreDriveComponent* UKRGA_CoreDriveSurge_Sword::GetCoreDriveComponent() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return nullptr;
	}

	return AvatarActor->FindComponentByClass<UKRCoreDriveComponent>();
}

void UKRGA_CoreDriveSurge_Sword::ApplyInvincibility()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		// 무적 + 하이퍼아머 둘 다 적용
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_DAMAGE);
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
		ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVESURGE);

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Invincibility applied"));
#endif
	}
}

void UKRGA_CoreDriveSurge_Sword::RemoveInvincibility()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_DAMAGE);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVESURGE);

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Invincibility removed"));
#endif
	}
}

void UKRGA_CoreDriveSurge_Sword::SetCollisionIgnorePawns(bool bIgnore)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (!Character)
	{
		return;
	}

	UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
	if (!CapsuleComp)
	{
		return;
	}

	if (bIgnore)
	{
		// 현재 Pawn 채널 응답 저장
		OriginalPawnResponse = CapsuleComp->GetCollisionResponseToChannel(ECC_Pawn);

		// Pawn 채널을 Overlap으로 변경하여 모든 Pawn 통과 가능
		CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Collision response to Pawn changed to Overlap"));
#endif
	}
	else
	{
		// 원래 콜리전 응답으로 복원
		CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, OriginalPawnResponse);

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Collision response to Pawn restored"));
#endif
	}
}

void UKRGA_CoreDriveSurge_Sword::SetupDashWarping()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	DashStartLocation = AvatarActor->GetActorLocation();

	// 락온 타겟 확인
	AActor* LockedTarget = UKRGA_LockOn::GetLockedTargetFor(AvatarActor);

	FVector DashDirection;

	if (LockedTarget)
	{
		// 타겟이 있으면 타겟 방향으로 대시 (수평으로만)
		FVector DirectionToTarget = UKRMotionWarpingLibrary::GetDirectionToActor(
			AvatarActor, LockedTarget, true);

		// 수평 방향만 사용 (Z축 제거)
		DashDirection = FVector(DirectionToTarget.X, DirectionToTarget.Y, 0.0f).GetSafeNormal();

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Dash toward locked target: %s"), *LockedTarget->GetName());
#endif
	}
	else
	{
		// 타겟이 없으면 전방으로 대시 (수평으로만)
		FVector Forward = AvatarActor->GetActorForwardVector();
		DashDirection = FVector(Forward.X, Forward.Y, 0.0f).GetSafeNormal();
	}

	// 수평 대시 (같은 높이 유지)
	DashEndLocation = DashStartLocation + DashDirection * DashDistance;
	DashEndLocation.Z = DashStartLocation.Z;  // Z좌표 고정

	// 모션 워핑 설정
	UKRMotionWarpingLibrary::SetWarpTargetByDirection(
		AvatarActor,
		WarpTargetName,
		DashDirection,
		DashDistance,
		LockedTarget != nullptr  // 락온 타겟이 있으면 회전
	);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Dash Direction: %s, Start: %s, End: %s"),
		*DashDirection.ToString(), *DashStartLocation.ToString(), *DashEndLocation.ToString());
#endif
}

void UKRGA_CoreDriveSurge_Sword::PerformDashDamage()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return;
	}

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxHalfExtent);

	// 시작점에서 종료점까지 Box Sweep
	bool bHit = World->SweepMultiByChannel(
		HitResults,
		DashStartLocation,
		DashEndLocation,
		AvatarActor->GetActorQuat(),
		ECC_Pawn,
		BoxShape,
		QueryParams
	);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActors.Contains(HitActor))
			{
				continue;
			}

			// ASC 보유 대상에게만 데미지
			UAbilitySystemComponent* TargetASC =
				UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			if (!TargetASC)
			{
				continue;
			}

			// 자기 자신 제외
			if (TargetASC == GetAbilitySystemComponentFromActorInfo())
			{
				continue;
			}

			ApplyDamageToTarget(HitActor, Hit);
			ExecuteHitCue(Hit.ImpactPoint);
			HitActors.Add(HitActor);

#if !UE_BUILD_SHIPPING
			UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Hit: %s"), *HitActor->GetName());
#endif
		}
	}
}

void UKRGA_CoreDriveSurge_Sword::ApplyDamageToTarget(AActor* TargetActor, const FHitResult& HitResult)
{
	if (!DamageEffectClass)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[CoreDriveSurge] DamageEffectClass is not set!"));
#endif
		return;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!SourceASC || !TargetASC)
	{
		return;
	}

	// 기본 공격력 가져오기
	float BaseAttackPower = 0.f;
	if (const UKRCombatCommonSet* CombatSet = SourceASC->GetSet<UKRCombatCommonSet>())
	{
		BaseAttackPower = CombatSet->GetAttackPower();
	}

	// 데미지 계산
	const float FinalDamage = BaseAttackPower * DamageMultiplier;

	// GE 적용
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, GetAbilityLevel());
	if (SpecHandle.IsValid())
	{
		// SetByCaller로 데미지 전달
		SpecHandle.Data->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_BASEDAMAGE, FinalDamage);

		// HitResult 정보 추가
		FGameplayEffectContextHandle ContextHandle = SpecHandle.Data->GetContext();
		ContextHandle.AddHitResult(HitResult, true);

		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}

void UKRGA_CoreDriveSurge_Sword::ExecuteDashStartCue()
{
	if (!DashStartCueTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	FGameplayCueParameters CueParams;
	CueParams.Location = DashStartLocation;
	CueParams.Normal = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	ASC->ExecuteGameplayCue(DashStartCueTag, CueParams);
}

void UKRGA_CoreDriveSurge_Sword::ExecuteHitCue(const FVector& HitLocation)
{
	if (!DashHitCueTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	FGameplayCueParameters CueParams;
	CueParams.Location = HitLocation;

	ASC->ExecuteGameplayCue(DashHitCueTag, CueParams);
}

void UKRGA_CoreDriveSurge_Sword::PerformDashDamageFromNotify()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return;
	}

	// 현재 위치 가져오기
	const FVector CurrentLocation = AvatarActor->GetActorLocation();

	// 시작점에서 현재 위치까지 Sweep으로 히트 체크 (이동 경로 전체 검사)
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxHalfExtent);

	bool bHit = World->SweepMultiByChannel(
		HitResults,
		DashStartLocation,
		CurrentLocation,
		AvatarActor->GetActorQuat(),
		ECC_Pawn,
		BoxShape,
		QueryParams
	);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActors.Contains(HitActor))
			{
				continue;
			}

			UAbilitySystemComponent* TargetASC =
				UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
			if (!TargetASC)
			{
				continue;
			}

			// 자기 자신 제외
			if (TargetASC == GetAbilitySystemComponentFromActorInfo())
			{
				continue;
			}

			ApplyDamageToTarget(HitActor, Hit);
			ExecuteHitCue(Hit.ImpactPoint);
			HitActors.Add(HitActor);

#if !UE_BUILD_SHIPPING
			UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] NotifyHit (Sweep): %s at %s"),
				*HitActor->GetName(), *Hit.ImpactPoint.ToString());
#endif
		}
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Sweep from %s to %s, Hits: %d"),
		*DashStartLocation.ToString(), *CurrentLocation.ToString(), HitResults.Num());
#endif
}

void UKRGA_CoreDriveSurge_Sword::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGA_CoreDriveSurge_Sword::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGA_CoreDriveSurge_Sword::SpawnDashTrailEffect()
{
	// StarDashData가 설정되지 않은 경우 이펙트 없이 진행 (선택적 기능)
	if (!StarDashData)
	{
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	const FKRStarDashEffectSettings& EffectSettings = StarDashData->Effects;
	const FKRStarDashTimingSettings& TimingSettings = StarDashData->Timing;

	// 번개 스플라인 이펙트 스폰
	if (EffectSettings.bEnableLightning && EffectSettings.SplineLightningClass)
	{
		UKRStarEffectSpawner::SpawnSplineLightningActor(
			AvatarActor,
			EffectSettings.SplineLightningClass,
			DashStartLocation,
			DashEndLocation,
			TimingSettings.LineDrawDuration,
			-1  // Random Seed
		);

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Lightning trail effect spawned"));
#endif
	}
}

void UKRGA_CoreDriveSurge_Sword::SetupHitCheckEventListener()
{
	if (!HitCheckEventTag.IsValid())
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[CoreDriveSurge] HitCheckEventTag is not valid!"));
#endif
		return;
	}

	// WaitGameplayEvent Task 생성
	// OnlyTriggerOnce = false: 대시 중 여러 번 히트 체크 이벤트 수신 필요
	// OnlyMatchExact = true: 정확한 태그 매칭
	HitCheckEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitCheckEventTag,
		nullptr,  // OptionalExternalTarget
		false,    // OnlyTriggerOnce = false (여러 번 수신)
		true      // OnlyMatchExact = true
	);

	if (HitCheckEventTask)
	{
		HitCheckEventTask->EventReceived.AddDynamic(this, &ThisClass::OnHitCheckEventReceived);
		HitCheckEventTask->ReadyForActivation();

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] HitCheck event listener setup complete. Tag: %s"), *HitCheckEventTag.ToString());
#endif
	}
}

void UKRGA_CoreDriveSurge_Sword::OnHitCheckEventReceived(FGameplayEventData Payload)
{
	// GameplayEvent 수신 시 대시 데미지 처리
	PerformDashDamageFromNotify();
}

// ─────────────────────────────────────────────────────────────────────────────
// 이동 제어
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveSurge_Sword::DisableMovement()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
	if (!MovementComp)
	{
		return;
	}

	// 원래 상태 저장
	OriginalGravityScale = MovementComp->GravityScale;
	OriginalMovementMode = MovementComp->MovementMode;

	// 중력 비활성화 (수평 대시를 위해)
	MovementComp->GravityScale = 0.0f;

	// Flying 모드로 변경 (바닥 충돌 무시)
	MovementComp->SetMovementMode(MOVE_Flying);

	// 플레이어 입력 이동 비활성화
	MovementComp->StopMovementImmediately();

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Movement disabled - GravityScale: 0, Mode: Flying"));
#endif
}

void UKRGA_CoreDriveSurge_Sword::EnableMovement()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	ACharacter* Character = Cast<ACharacter>(AvatarActor);
	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
	if (!MovementComp)
	{
		return;
	}

	// 원래 상태 복원
	MovementComp->GravityScale = OriginalGravityScale;
	MovementComp->SetMovementMode(OriginalMovementMode);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Movement enabled - GravityScale: %.2f, Mode restored"), OriginalGravityScale);
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// 디버그 시각화
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveSurge_Sword::DrawDebugHitBox()
{
	if (!bShowDebugShape)
	{
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return;
	}

	// 시작점과 끝점 사이의 경로를 따라 박스 시각화
	const FVector PathCenter = (DashStartLocation + DashEndLocation) * 0.5f;
	const FVector PathExtent = FVector(
		FVector::Dist(DashStartLocation, DashEndLocation) * 0.5f,
		BoxHalfExtent.Y,
		BoxHalfExtent.Z
	);

	// 대시 방향에 맞춰 회전
	const FVector DashDirection = (DashEndLocation - DashStartLocation).GetSafeNormal();
	const FRotator BoxRotation = DashDirection.Rotation();

	// 대시 경로 박스
	DrawDebugBox(
		World,
		PathCenter,
		PathExtent,
		FQuat(BoxRotation),
		FColor::Cyan,
		false,
		DebugDrawDuration,
		0,
		3.0f
	);

	// 시작점
	DrawDebugSphere(
		World,
		DashStartLocation,
		30.0f,
		12,
		FColor::Green,
		false,
		DebugDrawDuration,
		0,
		2.0f
	);

	// 끝점
	DrawDebugSphere(
		World,
		DashEndLocation,
		30.0f,
		12,
		FColor::Red,
		false,
		DebugDrawDuration,
		0,
		2.0f
	);

	// 대시 방향 화살표
	DrawDebugDirectionalArrow(
		World,
		DashStartLocation,
		DashEndLocation,
		50.0f,
		FColor::Yellow,
		false,
		DebugDrawDuration,
		0,
		3.0f
	);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveSurge] Debug shapes drawn - Duration: %.1fs"), DebugDrawDuration);
#endif
}
