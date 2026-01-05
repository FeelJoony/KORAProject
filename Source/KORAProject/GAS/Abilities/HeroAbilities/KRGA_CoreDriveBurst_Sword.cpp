#include "GAS/Abilities/HeroAbilities/KRGA_CoreDriveBurst_Sword.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/KRCoreDriveComponent.h"
#include "Components/KRCameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/KRCameraMode.h"
#include "Camera/KRCameraMode_Burst.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Abilities/HeroAbilities/KRGA_LockOn.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRMotionWarpingLibrary.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "TimerManager.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

UKRGA_CoreDriveBurst_Sword::UKRGA_CoreDriveBurst_Sword(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(KRTAG_ABILITY_ATTACK_COREDRIVEBURST);
	SetAssetTags(Tags);

	ActivationPolicy = EKRAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	// 기본 Cue 태그 설정
	StartupCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Burst.Startup"), false);
	AttackCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Burst.Attack"), false);
	HitCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Burst.Hit"), false);
	FinishCueTag = FGameplayTag::RequestGameplayTag(FName("GameplayCue.CoreDrive.Burst.Finish"), false);
}

void UKRGA_CoreDriveBurst_Sword::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	UKRCoreDriveComponent* CoreDriveComp = GetCoreDriveComponent();
	if (!CoreDriveComp || !CoreDriveComp->CanUseCoreDriveBurst())
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[CoreDriveBurst] CoreDrive not full - cannot use Burst"));
#endif
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	CoreDriveComp->ConsumeAllCoreDrive();

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		BurstAnchorLocation = AvatarActor->GetActorLocation();
		BurstAnchorRotation = AvatarActor->GetActorRotation();
	}
	
	SetIgnorePawnCollision(true);
	DisableMovement();
	PushCameraMode();
	TransitionToPhase(EKRCoreDriveBurstPhase::Startup);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveBurst] Activated - Starting Startup Phase"));
#endif
}

void UKRGA_CoreDriveBurst_Sword::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 타이머 정리
	StopMultiHitSequence();

	// 워프 타겟 정리
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		UKRMotionWarpingLibrary::RemoveWarpTarget(AvatarActor, FName("BurstTarget"));
	}

	// 현재 페이즈에 따라 정리
	switch (CurrentPhase)
	{
	case EKRCoreDriveBurstPhase::Startup:
		RemoveHyperArmor();
		break;
	case EKRCoreDriveBurstPhase::Attack:
		RemoveFullInvincibility();
		break;
	default:
		break;
	}

	SetIgnorePawnCollision(false);
	
	// 이동 활성화
	EnableMovement();

	// 카메라 모드 제거
	RemoveCameraMode();
	
	CurrentPhase = EKRCoreDriveBurstPhase::None;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UKRCoreDriveComponent* UKRGA_CoreDriveBurst_Sword::GetCoreDriveComponent() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return nullptr;
	}

	return AvatarActor->FindComponentByClass<UKRCoreDriveComponent>();
}

// ─────────────────────────────────────────────────────────────────────────────
// 페이즈 전환
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::TransitionToPhase(EKRCoreDriveBurstPhase NewPhase)
{
	// 이전 페이즈 정리
	switch (CurrentPhase)
	{
	case EKRCoreDriveBurstPhase::Startup:
		RemoveHyperArmor();
		break;
	case EKRCoreDriveBurstPhase::Attack:
		StopMultiHitSequence();
		RemoveFullInvincibility();
		break;
	default:
		break;
	}

	CurrentPhase = NewPhase;

	// 새 페이즈 시작
	switch (NewPhase)
	{
	case EKRCoreDriveBurstPhase::Startup:
		StartStartupPhase();
		break;
	case EKRCoreDriveBurstPhase::Attack:
		StartAttackPhase();
		break;
	case EKRCoreDriveBurstPhase::Finished:
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		break;
	default:
		break;
	}

#if !UE_BUILD_SHIPPING
	static const TCHAR* PhaseNames[] = { TEXT("None"), TEXT("Startup"), TEXT("Attack"), TEXT("Finished") };
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveBurst] Transitioned to Phase: %s"), PhaseNames[static_cast<int32>(NewPhase)]);
#endif
}

void UKRGA_CoreDriveBurst_Sword::StartStartupPhase()
{
	ApplyHyperArmor();
	ExecuteCue(StartupCueTag);

	if (StartupMontage)
	{
		PlayMontageWithCallback(StartupMontage, &ThisClass::OnStartupMontageCompleted);
	}
	else
	{
		// 몽타주 없으면 바로 Attack 페이즈로
		TransitionToPhase(EKRCoreDriveBurstPhase::Attack);
	}
}

void UKRGA_CoreDriveBurst_Sword::StartAttackPhase()
{
	ApplyFullInvincibility();
	ExecuteCue(AttackCueTag);

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		AActor* LockedTarget = UKRGA_LockOn::GetLockedTargetFor(AvatarActor);
		if (LockedTarget)
		{
			// 워핑은 유지 (첫 타격을 위해 방향 보정)
			FVector DirectionToTarget = UKRMotionWarpingLibrary::GetDirectionToActor(AvatarActor, LockedTarget, true);
			FRotator TargetRotation = UKRMotionWarpingLibrary::GetRotationFromDirection(DirectionToTarget);

			UKRMotionWarpingLibrary::SetWarpTargetRotation(AvatarActor, FName("BurstTarget"), TargetRotation);
			
			// [중요] 타겟이 있다면 앵커 회전도 타겟 방향으로 보정
			BurstAnchorRotation = TargetRotation; 
		}
	}

	CurrentHitIndex = 0;
	StartMultiHitSequence();

	if (AttackMontage)
	{
		PlayMontageWithCallback(AttackMontage, &ThisClass::OnAttackMontageCompleted);
	}
}


// ─────────────────────────────────────────────────────────────────────────────
// 상태 태그 관리
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::ApplyHyperArmor()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		// 히트리액션만 면역 (데미지는 받음)
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
		ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVEBURST);
	}
}

void UKRGA_CoreDriveBurst_Sword::RemoveHyperArmor()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVEBURST);
	}
}

void UKRGA_CoreDriveBurst_Sword::ApplyFullInvincibility()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_DAMAGE);
		ASC->AddLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
		ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVEBURST);
	}
}

void UKRGA_CoreDriveBurst_Sword::RemoveFullInvincibility()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_DAMAGE);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_IMMUNE_HITREACTION);
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_COREDRIVEBURST);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// 다단 히트
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::StartMultiHitSequence()
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

	// 타이머로 일정 간격 다단 히트
	World->GetTimerManager().SetTimer(
		MultiHitTimerHandle,
		this,
		&ThisClass::PerformMultiHit,
		AttackConfig.HitInterval,
		true,  // 반복
		0.0f   // 즉시 시작
	);
}

void UKRGA_CoreDriveBurst_Sword::StopMultiHitSequence()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	UWorld* World = AvatarActor->GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(MultiHitTimerHandle);
		World->GetTimerManager().ClearTimer(FinalHitTimerHandle);
	}
}

void UKRGA_CoreDriveBurst_Sword::PerformMultiHit()
{
	const int32 NormalHitCount = AttackConfig.HitCount - 1;

	if (CurrentHitIndex >= NormalHitCount)
	{
		AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (AvatarActor && AvatarActor->GetWorld())
		{
			AvatarActor->GetWorld()->GetTimerManager().ClearTimer(MultiHitTimerHandle);
		}
		ScheduleFinalHit();
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->GetWorld()) return;

	HitActorsThisHit.Empty();

	// [난무 단계 1~5타]
	// 캐릭터가 애니메이션(루트모션)으로 움직여도 판정 박스는 'Anchor'에 고정합니다.
	FVector Origin = BurstAnchorLocation + BurstAnchorRotation.Vector() * AttackConfig.ForwardOffset;
	FQuat Rotation = BurstAnchorRotation.Quaternion();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	TArray<FOverlapResult> OverlapResults;
	UWorld* World = AvatarActor->GetWorld();

	if (AttackConfig.AttackShape == EKRAttackShapeType::Sphere)
	{
		World->OverlapMultiByChannel(
			OverlapResults, Origin, FQuat::Identity, ECC_Pawn,
			FCollisionShape::MakeSphere(AttackConfig.SphereRadius), QueryParams);
	}
	else if (AttackConfig.AttackShape == EKRAttackShapeType::Box)
	{
		World->OverlapMultiByChannel(
			OverlapResults, Origin, Rotation, ECC_Pawn,
			FCollisionShape::MakeBox(AttackConfig.BoxExtent), QueryParams);
	}

	DrawDebugAttackShape();

	const float CurrentMultiplier = AttackConfig.DamageMultiplierPerHit;

	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor || HitActorsThisHit.Contains(HitActor)) continue;

		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (!TargetASC || TargetASC == GetAbilitySystemComponentFromActorInfo()) continue;

		ApplyDamageToTarget(HitActor, CurrentMultiplier);
		ExecuteCueAtLocation(HitCueTag, HitActor->GetActorLocation());
		HitActorsThisHit.Add(HitActor);
	}

	CurrentHitIndex++;
}

void UKRGA_CoreDriveBurst_Sword::ScheduleFinalHit()
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

	// 마지막 히트는 추가 딜레이 후 실행 (애니메이션 피니셔와 동기화)
	World->GetTimerManager().SetTimer(
		FinalHitTimerHandle,
		this,
		&ThisClass::PerformFinalHit,
		AttackConfig.FinalHitDelay,
		false  // 반복 안함
	);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveBurst] Final hit scheduled after %.2f seconds"), AttackConfig.FinalHitDelay);
#endif
}

void UKRGA_CoreDriveBurst_Sword::PerformFinalHit()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !AvatarActor->GetWorld()) return;

	HitActorsThisHit.Empty();

	// [수정 완료 - 피니시 단계 6타]
	// 사용자 요청 반영: 피니시 공격도 캐릭터 현재 위치가 아닌, 처음 지정된 'Anchor'를 기준으로 합니다.
	// 즉, 1~5타와 동일한 공격 범위를 가집니다. (캐릭터가 엉뚱한 곳에 착지해도 판정은 원래 위치에 남음)
	
	FVector Origin = BurstAnchorLocation + BurstAnchorRotation.Vector() * AttackConfig.ForwardOffset;
	FQuat Rotation = BurstAnchorRotation.Quaternion(); 

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	TArray<FOverlapResult> OverlapResults;
	UWorld* World = AvatarActor->GetWorld();

	if (AttackConfig.AttackShape == EKRAttackShapeType::Sphere)
	{
		// 피니시는 보통 범위가 더 크므로 1.5배 (선택 사항 - Config로 빼도 됨)
		World->OverlapMultiByChannel(OverlapResults, Origin, FQuat::Identity, ECC_Pawn,
			FCollisionShape::MakeSphere(AttackConfig.SphereRadius * 1.5f), QueryParams);
	}
	else if (AttackConfig.AttackShape == EKRAttackShapeType::Box)
	{
		World->OverlapMultiByChannel(OverlapResults, Origin, Rotation, ECC_Pawn,
			FCollisionShape::MakeBox(AttackConfig.BoxExtent * 1.5f), QueryParams);
	}

	DrawDebugAttackShape();

	const float FinalMultiplier = AttackConfig.FinalHitDamageMultiplier;

	for (const FOverlapResult& Overlap : OverlapResults)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor || HitActorsThisHit.Contains(HitActor)) continue;
		
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (!TargetASC || TargetASC == GetAbilitySystemComponentFromActorInfo()) continue;

		ApplyDamageToTarget(HitActor, FinalMultiplier);
		ExecuteCueAtLocation(HitCueTag, HitActor->GetActorLocation());
		HitActorsThisHit.Add(HitActor);
	}

	CurrentHitIndex++;
}

void UKRGA_CoreDriveBurst_Sword::ApplyDamageToTarget(AActor* TargetActor, float DamageMultiplier)
{
	if (!DamageEffectClass)
	{
#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Warning, TEXT("[CoreDriveBurst] DamageEffectClass is not set!"));
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
		SpecHandle.Data->SetSetByCallerMagnitude(KRTAG_SETBYCALLER_BASEDAMAGE, FinalDamage);
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// GameplayCue
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::ExecuteCue(const FGameplayTag& CueTag)
{
	if (!CueTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	FGameplayCueParameters CueParams;
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		CueParams.Location = AvatarActor->GetActorLocation();
	}

	ASC->ExecuteGameplayCue(CueTag, CueParams);
}

void UKRGA_CoreDriveBurst_Sword::ExecuteCueAtLocation(const FGameplayTag& CueTag, const FVector& Location)
{
	if (!CueTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	FGameplayCueParameters CueParams;
	CueParams.Location = Location;

	ASC->ExecuteGameplayCue(CueTag, CueParams);
}

// ─────────────────────────────────────────────────────────────────────────────
// 몽타주 재생
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::PlayMontageWithCallback(UAnimMontage* Montage, void(UKRGA_CoreDriveBurst_Sword::*Callback)())
{
	if (!Montage)
	{
		return;
	}

	// 기존 태스크 정리
	if (MontageTask)
	{
		MontageTask->EndTask();
		MontageTask = nullptr;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		Montage,
		1.0f,
		NAME_None,
		false
	);

	if (MontageTask)
	{
		// 콜백 타입에 따라 바인딩
		if (Callback == &ThisClass::OnStartupMontageCompleted)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnStartupMontageCompleted);
			MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnStartupMontageCompleted);
		}
		else if (Callback == &ThisClass::OnAttackMontageCompleted)
		{
			MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnAttackMontageCompleted);
			MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnAttackMontageCompleted);
		}

		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);

		MontageTask->ReadyForActivation();
	}
}

void UKRGA_CoreDriveBurst_Sword::SetIgnorePawnCollision(bool bIgnore)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (ACharacter* Character = Cast<ACharacter>(AvatarActor))
	{
		UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
		if (!Capsule) return;

		if (bIgnore)
		{
			// 현재 충돌 설정을 백업하고 Pawn 무시 설정
			PreviousCollisionResponses.Empty();
			PreviousCollisionResponses.Add(ECC_Pawn, Capsule->GetCollisionResponseToChannel(ECC_Pawn));
			
			Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			// 필요하다면 적 캐릭터 전용 채널(예: GameTraceChannel1)도 여기서 Ignore 처리
		}
		else
		{
			// 원래 설정으로 복구
			for (const auto& Pair : PreviousCollisionResponses)
			{
				Capsule->SetCollisionResponseToChannel(Pair.Key, Pair.Value);
			}
			PreviousCollisionResponses.Empty();
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// 몽타주 콜백
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::OnStartupMontageCompleted()
{
	TransitionToPhase(EKRCoreDriveBurstPhase::Attack);
}

void UKRGA_CoreDriveBurst_Sword::OnAttackMontageCompleted()
{
	// 공격 + 종료가 합쳐진 몽타주이므로 바로 Finished로 전환
	ExecuteCue(FinishCueTag);
	TransitionToPhase(EKRCoreDriveBurstPhase::Finished);
}

void UKRGA_CoreDriveBurst_Sword::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

// ─────────────────────────────────────────────────────────────────────────────
// 이동 제어
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::DisableMovement()
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

	if (bUseRootMotion)
	{
		// 루트 모션 사용 시: 플레이어 입력으로 인한 이동만 막고, 루트 모션은 허용
		// 현재 속도만 멈추고 이동 모드는 유지
		MovementComp->StopMovementImmediately();

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveBurst] Movement input disabled (root motion enabled)"));
#endif
	}
	else
	{
		// 루트 모션 미사용 시: 이동 완전 비활성화
		MovementComp->StopMovementImmediately();
		MovementComp->DisableMovement();

#if !UE_BUILD_SHIPPING
		UE_LOG(LogTemp, Log, TEXT("[CoreDriveBurst] Movement fully disabled"));
#endif
	}
}

void UKRGA_CoreDriveBurst_Sword::EnableMovement()
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

	// 루트 모션 미사용 시에만 이동 모드 복원 필요
	if (!bUseRootMotion)
	{
		MovementComp->SetMovementMode(MOVE_Walking);
	}

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveBurst] Movement enabled"));
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// 카메라 모드
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::PushCameraMode()
{
	if (!bUseCameraMode || !BurstCameraModeClass) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	UKRCameraComponent* CameraComp = UKRCameraComponent::FindCameraComponent(AvatarActor);
	if (!CameraComp) return;

	CameraComp->PushCameraMode(BurstCameraModeClass);

	if (UKRCameraMode_Burst* BurstCameraMode = CameraComp->GetCameraModeInstance<UKRCameraMode_Burst>())
	{
		BurstCameraMode->SetPlayerActor(AvatarActor);
		// 카메라는 스킬 시전 시작 위치(Anchor)를 기준으로 잡습니다.
		BurstCameraMode->SetupBurstCamera(BurstAnchorLocation, BurstAnchorRotation.Vector());
	}
	CameraComp->SetCameraModeUpdateLocked(true);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveBurst] Camera mode pushed: %s"), *BurstCameraModeClass->GetName());
#endif
}

void UKRGA_CoreDriveBurst_Sword::RemoveCameraMode()
{
	if (!bUseCameraMode || !BurstCameraModeClass)
	{
		return;
	}

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	UKRCameraComponent* CameraComp = UKRCameraComponent::FindCameraComponent(AvatarActor);
	if (!CameraComp)
	{
		return;
	}

	// 카메라 모드 업데이트 잠금 해제
	CameraComp->SetCameraModeUpdateLocked(false);

	// Burst 카메라 모드 정리
	if (UKRCameraMode_Burst* BurstCameraMode = CameraComp->GetCameraModeInstance<UKRCameraMode_Burst>())
	{
		BurstCameraMode->ClearBurstCamera();
	}

	CameraComp->RemoveCameraMode(BurstCameraModeClass);

#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Log, TEXT("[CoreDriveBurst] Camera mode removed: %s"), *BurstCameraModeClass->GetName());
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// 디버그 시각화
// ─────────────────────────────────────────────────────────────────────────────

void UKRGA_CoreDriveBurst_Sword::DrawDebugAttackShape()
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

	// 공격 원점 계산
	const FVector Origin = BurstAnchorLocation + BurstAnchorRotation.Vector() * AttackConfig.ForwardOffset;

	FQuat Rotation = BurstAnchorRotation.Quaternion();
	
	// 히트 횟수에 따른 색상 변화
	const float ColorLerp = static_cast<float>(CurrentHitIndex) / static_cast<float>(AttackConfig.HitCount);
	const FColor DebugColor = FColor::MakeRedToGreenColorFromScalar(1.0f - ColorLerp);

	if (AttackConfig.AttackShape == EKRAttackShapeType::Sphere)
	{
		DrawDebugSphere(
			World,
			Origin,
			AttackConfig.SphereRadius,
			16,
			DebugColor,
			false,
			DebugDrawDuration,
			0,
			2.0f
		);
	}
	else if (AttackConfig.AttackShape == EKRAttackShapeType::Box)
	{
		DrawDebugBox(
			World,
			Origin,
			AttackConfig.BoxExtent,
			Rotation,
			DebugColor,
			false,
			DebugDrawDuration,
			0,
			2.0f
		);
	}

	// 캐릭터 위치 표시
	DrawDebugSphere(
		World,
		AvatarActor->GetActorLocation(),
		20.0f,
		8,
		FColor::White,
		false,
		DebugDrawDuration,
		0,
		1.0f
	);
}