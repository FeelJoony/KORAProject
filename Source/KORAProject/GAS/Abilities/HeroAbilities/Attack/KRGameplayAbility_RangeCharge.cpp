#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_RangeCharge.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GAS/Abilities/Tasks/KRAbilityTask_WaitTick.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"

UKRGameplayAbility_RangeCharge::UKRGameplayAbility_RangeCharge(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FullChargeTime = 1.0f;
}

const TArray<TObjectPtr<UAnimMontage>>* UKRGameplayAbility_RangeCharge::GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const
{
	if (!Entry.IsValid())
	{
		return nullptr;
	}
	return &Entry.GetChargeAttackMontages(); 
}

void UKRGameplayAbility_RangeCharge::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);

		return;
	}

	UAnimMontage* ChargeMontage = GetMontageFromEquipment(0);

	if (!ChargeMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);

		return;
	}
	
	bIsCharging = true;
	bIsFullyCharged = false;
	CurrentChargeTime = 0.0f;
	
	ChargePlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("ChargePhase"),
		ChargeMontage,
		1.0f,
		SectionName_Start, 
		false 
	);

	if (ChargePlayTask)
	{
		ChargePlayTask->OnBlendOut.AddDynamic(this, &ThisClass::OnChargePhaseEnded);
		ChargePlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnChargePhaseEnded);
		ChargePlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnChargePhaseEnded);
		ChargePlayTask->ReadyForActivation();
	}
	
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);

		return;
	}
	
	ChargeTickTask = UKRAbilityTask_WaitTick::WaitTick(this, FName("RangeChargeTick"));

	if (ChargeTickTask)
	{
		ChargeTickTask->OnTick.AddDynamic(this, &ThisClass::OnChargeTick);
		ChargeTickTask->ReadyForActivation();
	}
	
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UKRGameplayAbility_RangeCharge::OnChargeTick(float DeltaTime)
{
	if (!bIsCharging) return;

	CurrentChargeTime += DeltaTime;

	if (!bIsFullyCharged && CurrentChargeTime >= FullChargeTime)
	{
		bIsFullyCharged = true;
	}
}

void UKRGameplayAbility_RangeCharge::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	if (!bIsCharging) return;
	
	bIsCharging = false;
	if (ChargeTickTask)
	{
		ChargeTickTask->EndTask();
		ChargeTickTask = nullptr;
	}
	
	bool bSuccess = (CurrentChargeTime >= FullChargeTime);
	
	StartReleaseSequence(bSuccess);
}

void UKRGameplayAbility_RangeCharge::StartReleaseSequence(bool bIsFullCharge)
{
	if (ChargePlayTask)
	{
		ChargePlayTask->EndTask();
		ChargePlayTask = nullptr;
	}

	UAnimMontage* ChargeMontage = GetMontageFromEquipment(0);
	
	if (!ChargeMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

		return;
	}
	
	FRotator AimRot = GetFinalAimRotation(MaxRange);
	
	FireWeaponActor(AimRot);

	FName TargetSection = SectionName_Fire;
	if (bIsFullCharge)
	{
		TargetSection = FName("Fire_Full");
	}
	
	ReleasePlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		   this,
		   FName("ReleasePhase"),
		   ChargeMontage,
		   1.0f,
		   TargetSection,
		   true
		);

	if (ReleasePlayTask)
	{
		ReleasePlayTask->OnBlendOut.AddDynamic(this, &ThisClass::OnReleasePhaseEnded);
		ReleasePlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnReleasePhaseEnded);
		ReleasePlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnReleasePhaseEnded);
		ReleasePlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnReleasePhaseEnded);
		ReleasePlayTask->ReadyForActivation();
	}
	
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UKRGameplayAbility_RangeCharge::OnChargePhaseEnded()
{
	if (bIsCharging)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UKRGameplayAbility_RangeCharge::OnReleasePhaseEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGameplayAbility_RangeCharge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bIsCharging = false;
	
	if (ChargeTickTask)
	{
		ChargeTickTask->EndTask();
		ChargeTickTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}