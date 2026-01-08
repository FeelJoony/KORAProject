#include "KRGameplayAbility_MeleeAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/KRStaminaComponent.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRMotionWarpingLibrary.h"
#include "GAS/Abilities/HeroAbilities/KRGA_LockOn.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GameplayTag/KREventTag.h"
#include "GameplayTag/KRSetByCallerTag.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MotionWarpingComponent.h"

UKRGameplayAbility_MeleeAttack::UKRGameplayAbility_MeleeAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKRGameplayAbility_MeleeAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	CheckComboTimeout();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 스태미나 체크 및 소모
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (AvatarActor)
	{
		if (UKRStaminaComponent* StaminaComp = AvatarActor->FindComponentByClass<UKRStaminaComponent>())
		{
			if (!StaminaComp->HasEnoughStamina(AttackStaminaCost))
			{
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
				return;
			}
			StaminaComp->ConsumeStamina(AttackStaminaCost);
		}
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 조회
	UAnimMontage* MontageToPlay = GetCurrentMontage();
	if (!MontageToPlay)
	{
		// 콤보 리셋 후 다시 시도
		ResetCombo();
		MontageToPlay = GetCurrentMontage();

		if (!MontageToPlay)
		{
			UE_LOG(LogTemp, Warning, TEXT("[MeleeAttack] No montage found for combo index %d"), CurrentComboIndex);
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	}

	// 히트 대상 초기화
	HitActorsThisSwing.Empty();
	bIsHitCheckActive = false;

	// IncrementCombo 전에 현재 콤보 인덱스 저장 (AnimNotify에서 사용)
	ActiveComboIndex = CurrentComboIndex;

	// 모션 워핑 설정
	if (bUseMotionWarping)
	{
		SetupMotionWarping();
	}

	// 휘두르기 Cue 실행 (이펙트/사운드)
	ExecuteSwingCue();

	// 몽타주 재생
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("MeleeAttack"),
		MontageToPlay,
		1.0f,
		NAME_None,
		true
	);

	if (MontageTask)
	{
		MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		MontageTask->ReadyForActivation();
	}

	// GameplayEvent 리스너 설정 (AnimNotifyState에서 발송한 이벤트 수신)
	SetupHitCheckEventListeners();

	IncrementCombo();
}

void UKRGameplayAbility_MeleeAttack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 모션 워핑 타겟 정리
	if (bUseMotionWarping)
	{
		UKRMotionWarpingLibrary::RemoveWarpTarget(GetAvatarActorFromActorInfo(), WarpTargetName);
	}

	// 히트 체크 상태 초기화
	bIsHitCheckActive = false;
	HitActorsThisSwing.Empty();

	if (!bWasCancelled)
	{
		StartComboResetTimer();
	}
	else
	{
		ResetCombo();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ─────────────────────────────────────────────────────
// AnimNotify 호출용
// ─────────────────────────────────────────────────────

void UKRGameplayAbility_MeleeAttack::BeginHitCheck()
{
	HitActorsThisSwing.Empty();
	bIsHitCheckActive = true;
}

void UKRGameplayAbility_MeleeAttack::PerformHitCheck()
{
	if (!bIsHitCheckActive) return;

	TArray<FHitResult> HitResults;
	if (PerformShapeTrace(HitResults))
	{
		ProcessHitResults(HitResults);
	}
}

void UKRGameplayAbility_MeleeAttack::EndHitCheck()
{
	bIsHitCheckActive = false;
}

const FKRMeleeAttackConfig& UKRGameplayAbility_MeleeAttack::GetCurrentAttackConfig() const
{
	// ActiveComboIndex 사용 (IncrementCombo 전에 저장된 값)
	if (ComboAttackConfigs.IsValidIndex(ActiveComboIndex))
	{
		return ComboAttackConfigs[ActiveComboIndex];
	}
	return DefaultAttackConfig;
}

// ─────────────────────────────────────────────────────
// 몽타주 조회
// ─────────────────────────────────────────────────────

UKREquipmentManagerComponent* UKRGameplayAbility_MeleeAttack::GetEquipmentManager() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return nullptr;

	return AvatarActor->FindComponentByClass<UKREquipmentManagerComponent>();
}

UMotionWarpingComponent* UKRGameplayAbility_MeleeAttack::GetMotionWarpingComponent() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return nullptr;

	return AvatarActor->FindComponentByClass<UMotionWarpingComponent>();
}

UAnimMontage* UKRGameplayAbility_MeleeAttack::GetCurrentMontage() const
{
	UKREquipmentManagerComponent* EquipMgr = GetEquipmentManager();
	if (!EquipMgr) return nullptr;

	FGameplayTag ActiveWeaponTag = EquipMgr->GetActiveWeaponTypeTag();
	if (!ActiveWeaponTag.IsValid()) return nullptr;

	const FKRAppliedEquipmentEntry& Entry = EquipMgr->FindEntryByTag(ActiveWeaponTag);
	if (!Entry.IsValid()) return nullptr;
	
	const TArray<TObjectPtr<UAnimMontage>>* Montages = GetMontageArrayFromEntry(Entry);
	if (!Montages) return nullptr;

	if (Montages->IsValidIndex(CurrentComboIndex))
	{
		return (*Montages)[CurrentComboIndex];
	}

	return nullptr;
}

const TArray<TObjectPtr<UAnimMontage>>* UKRGameplayAbility_MeleeAttack::GetMontageArrayFromEntry(
	const FKRAppliedEquipmentEntry& Entry) const
{
	return &Entry.GetLightAttackMontages();
}

// ─────────────────────────────────────────────────────
// 모션 워핑
// ─────────────────────────────────────────────────────

void UKRGameplayAbility_MeleeAttack::SetupMotionWarping()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	AActor* TargetActor = FindBestTarget();

	if (TargetActor)
	{
		float CurrentDistance = FVector::Dist2D(
			AvatarActor->GetActorLocation(),
			TargetActor->GetActorLocation()
		);

		// 최소 접근 거리보다 가까우면 워핑 스킵
		if (CurrentDistance > Config.MinApproachDistance)
		{
			// 락온 시에도 설정된 WarpDistance만큼만 이동하도록 제한 (텔레포트 방지)
			// 단, 타겟보다 더 멀리 이동하지 않도록 거리 클램핑
			float DistToTarget = CurrentDistance - Config.MinApproachDistance;
			float FinalWarpDistance = FMath::Min(Config.WarpDistance, DistToTarget);

			FVector Direction = UKRMotionWarpingLibrary::GetDirectionToActor(AvatarActor, TargetActor, true);

			UKRMotionWarpingLibrary::SetWarpTargetByDirection(
				AvatarActor,
				WarpTargetName,
				Direction,
				FinalWarpDistance,
				true
			);
		}
		else
		{
			// 이미 가까우면 회전만
			UKRMotionWarpingLibrary::SetWarpTargetRotation(
				AvatarActor,
				WarpTargetName,
				UKRMotionWarpingLibrary::GetRotationFromDirection(
					UKRMotionWarpingLibrary::GetDirectionToActor(AvatarActor, TargetActor, true)
				)
			);
		}
	}
	else
	{
		// 타겟이 없으면 전방으로 워핑
		UKRMotionWarpingLibrary::SetWarpTargetByDirection(
			AvatarActor,
			WarpTargetName,
			AvatarActor->GetActorForwardVector(),
			Config.WarpDistance,
			false
		);
	}
}

AActor* UKRGameplayAbility_MeleeAttack::FindBestTarget() const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return nullptr;

	return UKRGA_LockOn::GetLockedTargetFor(AvatarActor);
}

// ─────────────────────────────────────────────────────
// Shape Trace
// ─────────────────────────────────────────────────────

bool UKRGameplayAbility_MeleeAttack::PerformShapeTrace(TArray<FHitResult>& OutHitResults) const
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
				0.0f
			);
		}
	}

	return bHit;
}

bool UKRGameplayAbility_MeleeAttack::PerformConeTrace(TArray<FHitResult>& OutHitResults) const
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return false;

	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	const FVector Origin = AvatarActor->GetActorLocation();
	const FVector Forward = AvatarActor->GetActorForwardVector();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	QueryParams.bReturnPhysicalMaterial = false;
	
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

bool UKRGameplayAbility_MeleeAttack::PerformBoxTrace(TArray<FHitResult>& OutHitResults) const
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

bool UKRGameplayAbility_MeleeAttack::PerformSphereTrace(TArray<FHitResult>& OutHitResults) const
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

bool UKRGameplayAbility_MeleeAttack::PerformCapsuleTrace(TArray<FHitResult>& OutHitResults) const
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

// ─────────────────────────────────────────────────────
// 히트 처리
// ─────────────────────────────────────────────────────

void UKRGameplayAbility_MeleeAttack::ProcessHitResults(const TArray<FHitResult>& HitResults)
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

void UKRGameplayAbility_MeleeAttack::ApplyHitToTarget(AActor* HitActor, const FHitResult& HitResult)
{
	if (!HitActor) return;

	// 1. 데미지 적용
	ApplyDamage(HitActor, HitResult);

	// 2. 히트 리액션 이벤트 전송 (피격자에게 알림)
	SendHitReactionEvent(HitActor, HitResult);

	// 3. 히트 Cue 실행 (공격자 측 이펙트/사운드)
	ExecuteHitCue(HitResult);

	// 4. 히트스톱 적용
	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	if (Config.bUseHitStop)
	{
		ApplyHitStop();
	}

	// 4. 카메라 쉐이크 적용
	if (Config.bUseCameraShake && Config.HitCameraShake)
	{
		ApplyCameraShake();
	}

	// 5. 코어드라이브 충전 이벤트 전송 (공격자에게)
	// 타겟이 유효한 전투 대상(ASC 보유)인 경우에만 충전
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (TargetASC)
	{
		AActor* AvatarActor = GetAvatarActorFromActorInfo();
		if (AvatarActor)
		{
			// 기본 공격력 * 스킬 배율을 이벤트 데이터로 전달
			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
			float AttackPower = 1.0f;
			if (SourceASC)
			{
				if (const UKRCombatCommonSet* CombatSet = SourceASC->GetSet<UKRCombatCommonSet>())
				{
					AttackPower = CombatSet->GetAttackPower();
				}
			}
			float BaseDamage = AttackPower * Config.DamageMultiplier;

			FGameplayEventData ChargeEvent;
			ChargeEvent.EventTag = KRTAG_EVENT_COREDRIVE_CHARGE_ONHIT;
			ChargeEvent.EventMagnitude = BaseDamage;
			ChargeEvent.Instigator = AvatarActor;
			ChargeEvent.Target = HitActor;

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				AvatarActor,
				KRTAG_EVENT_COREDRIVE_CHARGE_ONHIT,
				ChargeEvent
			);
		}
	}
}

void UKRGameplayAbility_MeleeAttack::ApplyDamage(AActor* TargetActor, const FHitResult& HitResult)
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

void UKRGameplayAbility_MeleeAttack::SendHitReactionEvent(AActor* TargetActor, const FHitResult& HitResult)
{
	if (!TargetActor) return;

	// 공격자 ASC 확인
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC) return;

	// 타겟 ASC 확인 (ASC가 없는 액터 - Floor 등 - 에게 이벤트 전송 시 에러 방지)
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC) return;

	FGameplayEventData EventData;
	EventData.EventTag = KRTAG_EVENT_COMBAT_HITREACTION;
	EventData.Instigator = GetAvatarActorFromActorInfo();
	EventData.Target = TargetActor;
	EventData.ContextHandle = SourceASC->MakeEffectContext();
	EventData.ContextHandle.AddHitResult(HitResult);

	// 히트 강도 + 넉백 거리 패킹하여 전달
	// EventMagnitude = HitIntensity(정수부) + KnockbackDistance * 0.001(소수부)
	// 예: HitIntensity=2, KnockbackDistance=150 → 2.150
	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	float PackedMagnitude = static_cast<float>(Config.HitIntensity) + (Config.KnockbackDistance * 0.001f);
	EventData.EventMagnitude = PackedMagnitude;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		TargetActor,
		KRTAG_EVENT_COMBAT_HITREACTION,
		EventData
	);
}

void UKRGameplayAbility_MeleeAttack::ExecuteSwingCue()
{
	if (!SwingCueTag.IsValid()) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	FGameplayCueParameters CueParams;
	CueParams.Location = AvatarActor->GetActorLocation();
	CueParams.Normal = AvatarActor->GetActorForwardVector();

	// 콤보 인덱스를 전달하여 콤보별 다른 이펙트/사운드 재생 가능
	CueParams.RawMagnitude = static_cast<float>(CurrentComboIndex);

	ASC->ExecuteGameplayCue(SwingCueTag, CueParams);
}

void UKRGameplayAbility_MeleeAttack::ExecuteHitCue(const FHitResult& HitResult)
{
	if (!HitCueTag.IsValid()) return;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	FGameplayCueParameters CueParams;
	CueParams.Location = HitResult.ImpactPoint;
	CueParams.Normal = HitResult.ImpactNormal;

	ASC->ExecuteGameplayCue(HitCueTag, CueParams);
}

void UKRGameplayAbility_MeleeAttack::ApplyHitStop()
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	UWorld* World = AvatarActor->GetWorld();
	if (!World) return;

	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();

	// 공격자 히트스톱
	AvatarActor->CustomTimeDilation = 0.01f;

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		[WeakActor = TWeakObjectPtr<AActor>(AvatarActor)]()
		{
			if (AActor* Actor = WeakActor.Get())
			{
				Actor->CustomTimeDilation = 1.0f;
			}
		},
		Config.HitStopDuration,
		false
	);
}

void UKRGameplayAbility_MeleeAttack::ApplyCameraShake()
{
	const FKRMeleeAttackConfig& Config = GetCurrentAttackConfig();
	if (!Config.HitCameraShake) return;

	APlayerController* PC = Cast<APlayerController>(GetControllerFromActorInfo());
	if (PC)
	{
		PC->ClientStartCameraShake(Config.HitCameraShake);
	}
}

// ─────────────────────────────────────────────────────
// 몽타주 콜백
// ─────────────────────────────────────────────────────

void UKRGameplayAbility_MeleeAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGameplayAbility_MeleeAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

// ─────────────────────────────────────────────────────
// GameplayEvent 리스너
// ─────────────────────────────────────────────────────

void UKRGameplayAbility_MeleeAttack::SetupHitCheckEventListeners()
{
	// Begin 이벤트 리스너 (한 번만 수신)
	HitCheckBeginTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		KRTAG_EVENT_MELEE_HITCHECK_BEGIN,
		nullptr,
		true,  // OnlyTriggerOnce = true
		true   // OnlyMatchExact = true
	);
	if (HitCheckBeginTask)
	{
		HitCheckBeginTask->EventReceived.AddDynamic(this, &ThisClass::OnHitCheckBeginEvent);
		HitCheckBeginTask->ReadyForActivation();
	}

	// Tick 이벤트 리스너 (여러 번 수신)
	HitCheckTickTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		KRTAG_EVENT_MELEE_HITCHECK_TICK,
		nullptr,
		false,  // OnlyTriggerOnce = false (여러 번 수신)
		true    // OnlyMatchExact = true
	);
	if (HitCheckTickTask)
	{
		HitCheckTickTask->EventReceived.AddDynamic(this, &ThisClass::OnHitCheckTickEvent);
		HitCheckTickTask->ReadyForActivation();
	}

	// End 이벤트 리스너 (한 번만 수신)
	HitCheckEndTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		KRTAG_EVENT_MELEE_HITCHECK_END,
		nullptr,
		true,  // OnlyTriggerOnce = true
		true   // OnlyMatchExact = true
	);
	if (HitCheckEndTask)
	{
		HitCheckEndTask->EventReceived.AddDynamic(this, &ThisClass::OnHitCheckEndEvent);
		HitCheckEndTask->ReadyForActivation();
	}
}

void UKRGameplayAbility_MeleeAttack::OnHitCheckBeginEvent(FGameplayEventData Payload)
{
	BeginHitCheck();
}

void UKRGameplayAbility_MeleeAttack::OnHitCheckTickEvent(FGameplayEventData Payload)
{
	PerformHitCheck();
}

void UKRGameplayAbility_MeleeAttack::OnHitCheckEndEvent(FGameplayEventData Payload)
{
	EndHitCheck();
}
