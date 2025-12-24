#include "KRGameplayAbility_ChargeAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GAS/Abilities/Tasks/KRAbilityTask_WaitTick.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRMotionWarpingLibrary.h"
#include "GameFramework/Character.h"

const FName UKRGameplayAbility_ChargeAttack::SECTION_START = FName("Start");
const FName UKRGameplayAbility_ChargeAttack::SECTION_FAIL = FName("Attack_Fail");
const FName UKRGameplayAbility_ChargeAttack::SECTION_SUCCESS = FName("Attack_Success");

UKRGameplayAbility_ChargeAttack::UKRGameplayAbility_ChargeAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 차지 공격 기본 설정
	DefaultAttackConfig.AttackShape = EKRAttackShapeType::Cone;
	DefaultAttackConfig.AttackRange = 250.0f;
	DefaultAttackConfig.ConeAngle = 120.0f;
	DefaultAttackConfig.DamageMultiplier = 1.0f;  // InputReleased에서 동적으로 변경
	DefaultAttackConfig.HitIntensity = EKRHitIntensity::Heavy;
	DefaultAttackConfig.bUseHitStop = true;
	DefaultAttackConfig.HitStopDuration = 0.08f;
}

void UKRGameplayAbility_ChargeAttack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	CheckComboTimeout();

	// AttackBase의 ActivateAbility를 직접 호출 (MeleeAttack의 자동 몽타주 재생 건너뛰기)
	UKRGameplayAbility_AttackBase::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주 조회
	CurrentChargeMontage = GetCurrentMontage();
	if (!CurrentChargeMontage)
	{
		ResetCombo();
		CurrentChargeMontage = GetCurrentMontage();

		if (!CurrentChargeMontage)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ChargeAttack] No montage found for combo index %d"), CurrentComboIndex);
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	}

	// 히트 대상 초기화
	HitActorsThisSwing.Empty();
	bIsHitCheckActive = false;

	// 차지 상태 초기화
	bIsChargeSuccess = false;
	bIsFullyCharged = false;
	bIsCharging = true;
	CurrentChargeTime = 0.0f;

	// 차지 페이즈 몽타주 재생 (Start 섹션부터)
	ChargePlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("ChargeAttack_Charge"),
		CurrentChargeMontage,
		1.0f,
		SECTION_START,
		false  // bStopWhenAbilityEnds = false (수동으로 제어)
	);

	if (ChargePlayTask)
	{
		ChargePlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnChargePhaseCompleted);
		ChargePlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnChargePhaseInterrupted);
		ChargePlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnChargePhaseInterrupted);
		ChargePlayTask->ReadyForActivation();
	}

	// 차지 틱 태스크 시작
	ChargeTickTask = UKRAbilityTask_WaitTick::WaitTick(this, FName("ChargeTick"));
	if (ChargeTickTask)
	{
		ChargeTickTask->OnTick.AddDynamic(this, &ThisClass::OnChargeTick);
		ChargeTickTask->ReadyForActivation();
	}

	IncrementCombo();
}

void UKRGameplayAbility_ChargeAttack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// 차지 틱 태스크 정리
	if (ChargeTickTask)
	{
		ChargeTickTask->EndTask();
		ChargeTickTask = nullptr;
	}

	// 모션 워핑 타겟 정리
	if (bUseMotionWarping)
	{
		UKRMotionWarpingLibrary::RemoveWarpTarget(GetAvatarActorFromActorInfo(), WarpTargetName);
	}

	// 히트 체크 상태 초기화
	bIsHitCheckActive = false;
	HitActorsThisSwing.Empty();
	bIsCharging = false;

	if (!bWasCancelled)
	{
		StartComboResetTimer();
	}
	else
	{
		ResetCombo();
	}

	// AttackBase의 EndAbility 직접 호출
	UKRGameplayAbility_AttackBase::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGameplayAbility_ChargeAttack::InputReleased(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (!bIsCharging)
	{
		return;
	}

	bIsCharging = false;

	// 차지 틱 태스크 종료
	if (ChargeTickTask)
	{
		ChargeTickTask->EndTask();
		ChargeTickTask = nullptr;
	}

	// 차지 실패 - 일찍 놓음
	bIsChargeSuccess = false;
	DefaultAttackConfig.DamageMultiplier = DamageMulti_Fail;
	DefaultAttackConfig.HitIntensity = EKRHitIntensity::Medium;

	StartReleaseMontage(false);
}

void UKRGameplayAbility_ChargeAttack::OnChargeTick(float DeltaTime)
{
	if (!bIsCharging)
	{
		return;
	}

	CurrentChargeTime += DeltaTime;

	// 풀 차지 달성
	if (!bIsFullyCharged && CurrentChargeTime >= TargetChargeTime)
	{
		bIsFullyCharged = true;
		bIsChargeSuccess = true;
		bIsCharging = false;

		DefaultAttackConfig.DamageMultiplier = DamageMulti_Success;
		DefaultAttackConfig.HitIntensity = EKRHitIntensity::Heavy;

		// 차지 틱 태스크 종료
		if (ChargeTickTask)
		{
			ChargeTickTask->EndTask();
			ChargeTickTask = nullptr;
		}

		// 차지 성공 - 자동 릴리즈
		StartReleaseMontage(true);
	}
}

void UKRGameplayAbility_ChargeAttack::StartReleaseMontage(bool bSuccess)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char || !CurrentChargeMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	// 차지 페이즈 몽타주 정리
	if (ChargePlayTask)
	{
		ChargePlayTask->EndTask();
		ChargePlayTask = nullptr;
	}

	// 현재 재생 중인 몽타주 중지
	if (UAnimInstance* AnimInst = Char->GetMesh() ? Char->GetMesh()->GetAnimInstance() : nullptr)
	{
		if (AnimInst->Montage_IsPlaying(CurrentChargeMontage))
		{
			AnimInst->Montage_Stop(0.1f, CurrentChargeMontage);
		}
	}

	// 모션 워핑 설정 (릴리즈 시점에)
	if (bUseMotionWarping)
	{
		SetupMotionWarping();
	}

	// 릴리즈 페이즈 몽타주 재생
	const FName ReleaseSection = bSuccess ? SECTION_SUCCESS : SECTION_FAIL;

	ReleasePlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("ChargeAttack_Release"),
		CurrentChargeMontage,
		1.0f,
		ReleaseSection,
		true
	);

	if (ReleasePlayTask)
	{
		ReleasePlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnReleasePhaseCompleted);
		ReleasePlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnReleasePhaseInterrupted);
		ReleasePlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnReleasePhaseInterrupted);
		ReleasePlayTask->OnBlendOut.AddDynamic(this, &ThisClass::OnReleasePhaseCompleted);
		ReleasePlayTask->ReadyForActivation();
	}
}

void UKRGameplayAbility_ChargeAttack::OnChargePhaseCompleted()
{
	// 차지 페이즈가 완료되면 (Start 섹션 끝) 릴리즈로 전환
	// 일반적으로 InputReleased나 풀차지에서 먼저 처리됨
}

void UKRGameplayAbility_ChargeAttack::OnChargePhaseInterrupted()
{
	if (!IsActive())
	{
		return;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGameplayAbility_ChargeAttack::OnReleasePhaseCompleted()
{
	if (!IsActive())
	{
		return;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGameplayAbility_ChargeAttack::OnReleasePhaseInterrupted()
{
	if (!IsActive())
	{
		return;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

const TArray<TObjectPtr<UAnimMontage>>* UKRGameplayAbility_ChargeAttack::GetMontageArrayFromEntry(
	const FKRAppliedEquipmentEntry& Entry) const
{
	return &Entry.GetChargeAttackMontages();
}
