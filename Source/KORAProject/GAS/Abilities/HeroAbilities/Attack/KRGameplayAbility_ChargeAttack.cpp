#include "KRGameplayAbility_ChargeAttack.h"
#include "Weapons/KRWeaponInstance.h"
#include "Components/KRCombatComponent.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"

UKRGameplayAbility_ChargeAttack::UKRGameplayAbility_ChargeAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DamageMulti_Fail = 1.f;
	DamageMulti_Success = 2.f;

	CurrentDamageMultiplier = 1.f;
}

void UKRGameplayAbility_ChargeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, ture);
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

	UAnimMontage* MontageToPlay = Weapon->GetChargeAttackMontage(CurrentComboIndex);

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

	bIsChargeSuccess = false;
	CurrentDamageMultiplier = DamageMulti_Fail;

	float TargetChargeTime = Weapon->GetWeaponChargeTime();

	if (TargetChargeTime <= KINDA_SMALL_NUMBER)
	{
		TargetChargeTime = 0.5f;
	}

	UAbilityTask_PlayMontageAndWait* PlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("ChargeAttack"),
		MontageToPlay,
		1.f,
		SECTION_START,
		true
		);

	if (PlayTask)
	{
		PlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		PlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		PlayTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
		PlayTask->ReadyForActivation();
	}

	UAbilityTask_WaitInputRelease* ReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	if (ReleaseTask)
	{
		ReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnKeyReleased);
		ReleaseTask->ReadyForActivation();
	}

	UAbilityTask_WaitDelay* ChargeTimerTask = UAbilityTask_WaitDelay::WaitDelay(this, TargetChargeTime);
	if (ChargeTimerTask)
	{
		ChargeTimerTask->OnFinish.AddDynamic(this, &ThisClass::OnChargeCompleted);
		ChargeTimerTask->ReadyForActivation();
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

void UKRGameplayAbility_ChargeAttack::OnKeyReleased(float TimeHeld)
{
	if (bIsChargeSuccess) return;;

	if (ACharacter* MyChar = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance())
		{
			if (AnimInst->Montage_IsPlaying(nullptr))
			{
				AnimInst->Montage_JumpToSection(SECTION_FAIL);
			}
		}
	}
}

void UKRGameplayAbility_ChargeAttack::OnChargeCompleted()
{
	bIsChargeSuccess = true;
	CurrentDamageMultiplier = DamageMulti_Success;

	if (ACharacter* MyChar = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance())
		{
			if (AnimInst->Montage_IsPlaying(nullptr))
			{
				AnimInst->Montage_JumpToSection(SECTION_SUCCESS);
			}
		}
	}

	// 차징 완료 이펙트, 사운드(GameplayCue 추가 활용 가능)
}

void UKRGameplayAbility_ChargeAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGameplayAbility_ChargeAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
