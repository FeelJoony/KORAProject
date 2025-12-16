#include "KRGameplayAbility_ChargeAttack.h"

#include "IMediaControls.h"
#include "Weapons/KRWeaponInstance.h"
#include "Components/KRCombatComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "GAS/Abilities/Tasks/KRAbilityTask_WaitTick.h"

const FName UKRGameplayAbility_ChargeAttack::SECTION_START   = FName("Start");
const FName UKRGameplayAbility_ChargeAttack::SECTION_FAIL    = FName("Attack_Fail");
const FName UKRGameplayAbility_ChargeAttack::SECTION_SUCCESS = FName("Attack_Success");

UKRGameplayAbility_ChargeAttack::UKRGameplayAbility_ChargeAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentDamageMultiplier = 1.f;
	DamageMulti_Fail = 1.f;
	DamageMulti_Success = 2.f;
	bIsChargeSuccess = false;
	bIsFullyCharged = false;
	bIsCharging = false;
	TargetChargeTime = 0.f;
	CurrentChargeTime = 0.f;
}

void UKRGameplayAbility_ChargeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	CheckComboTimeout();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UKRWeaponInstance* Weapon = nullptr;
	if (UKRCombatComponent* CombatComp = GetCombatComponentFromActorInfo())
	{
		Weapon = CombatComp->GetCurrentWeaponInstance();
	}

	if (!Weapon)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	/*UAnimMontage* MontageToPlay = Weapon->GetChargeAttackMontage(CurrentComboIndex);

	if (!MontageToPlay)
	{
		ResetCombo();
		MontageToPlay = Weapon->GetChargeAttackMontage(CurrentComboIndex);

		if (!MontageToPlay)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}

	CurrentChargeMontage = MontageToPlay;

	// 차지 상태 초기화
	bIsChargeSuccess = false;
	bIsFullyCharged = false;
	bIsCharging = true;
	CurrentChargeTime = 0.f;
	CurrentDamageMultiplier = DamageMulti_Fail;

	TargetChargeTime = Weapon->GetWeaponChargeTime();
	if (TargetChargeTime <= KINDA_SMALL_NUMBER)
	{
		TargetChargeTime = 1.f;
	}*/
	
	ChargePlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("ChargeAttack_Charge"),
		CurrentChargeMontage,
		1.f,
		SECTION_START,
		false);

	if (ChargePlayTask)
	{
		ChargePlayTask->ReadyForActivation();
	}

	ChargeTickTask = UKRAbilityTask_WaitTick::WaitTick(this, FName("ChargeTick"));
	if (ChargeTickTask)
	{
		ChargeTickTask->OnTick.AddDynamic(this, &ThisClass::OnChargeTick);
		ChargeTickTask->ReadyForActivation();
	}
	
	IncrementCombo();
}

void UKRGameplayAbility_ChargeAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
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

void UKRGameplayAbility_ChargeAttack::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (!bIsCharging)
	{
		return;
	}

	bIsCharging = false;

	if (ChargeTickTask)
	{
		ChargeTickTask->EndTask();
		ChargeTickTask = nullptr;
	}

	bIsChargeSuccess = false;
	CurrentDamageMultiplier = DamageMulti_Fail;

	StartReleaseMontage(false);
}

void UKRGameplayAbility_ChargeAttack::OnChargeTick(float DeltaTime)
{
	if (!bIsCharging)
	{
		return;
	}

	CurrentChargeTime += DeltaTime;

	if (!bIsFullyCharged && CurrentChargeTime >= TargetChargeTime)
	{
		bIsFullyCharged = true;
		bIsChargeSuccess =true;
		bIsCharging = false;
		CurrentDamageMultiplier = DamageMulti_Success;

		if (ChargeTickTask)
		{
			ChargeTickTask->EndTask();
			ChargeTickTask = nullptr;
		}

		StartReleaseMontage(true);
	}
}



void UKRGameplayAbility_ChargeAttack::OnMontageCompleted()
{
	if (!IsActive())
	{
		return;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGameplayAbility_ChargeAttack::OnMontageInterrupted()
{
	if (!IsActive())
	{
		return;
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGameplayAbility_ChargeAttack::StartReleaseMontage(bool bSuccess)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char || !CurrentChargeMontage)
	{
		return;
	}

	if (ChargePlayTask)
	{
		ChargePlayTask->EndTask();
		ChargePlayTask = nullptr;
	}
	
	if (UAnimInstance* AnimInst = Char->GetMesh() ? Char->GetMesh()->GetAnimInstance() : nullptr)
	{
		if (AnimInst->Montage_IsPlaying(CurrentChargeMontage))
		{
			AnimInst->Montage_Stop(0.1f, CurrentChargeMontage);
		}
	}

	const FName ReleaseSection = bSuccess ? SECTION_SUCCESS : SECTION_FAIL;

	ReleasePlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("ChargeAttack_Release"),
		CurrentChargeMontage,
		1.f,
		ReleaseSection,
		true);

	if (ReleasePlayTask)
	{
		ReleasePlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		ReleasePlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		ReleasePlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		ReleasePlayTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		ReleasePlayTask->ReadyForActivation();
	}
}
