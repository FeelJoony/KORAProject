#include "KRGameplayAbility_LightAttack.h"
#include "Weapons/KRWeaponInstance.h"
#include "Components/KRCombatComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UKRGameplayAbility_LightAttack::UKRGameplayAbility_LightAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CurrentDamageMultiplier = 1.f;
}

void UKRGameplayAbility_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	/*UAnimMontage* MontageToPlay = Weapon->GetLightAttackMontage(CurrentComboIndex);

	if (!MontageToPlay)
	{
		ResetCombo();
		MontageToPlay = Weapon->GetLightAttackMontage(CurrentComboIndex);

		if (!MontageToPlay)
		{
			UE_LOG(LogTemp, Warning, TEXT("LightAttack: No montages found in WeaponInstance!"));
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
			return;
		}
	}

	CurrentMontage = MontageToPlay;

	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("LightAttack"),
		MontageToPlay,
		1.f,
		NAME_None,
		true
		);

	if (Task)
	{
		Task->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		Task->OnCancelled.AddDynamic(this, &ThisClass::OnMontageInterrupted);
		Task->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);

		Task->ReadyForActivation();
	}*/

	IncrementCombo();
}

void UKRGameplayAbility_LightAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
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

void UKRGameplayAbility_LightAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGameplayAbility_LightAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
