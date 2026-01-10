#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_RangeCharge.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GAS/Abilities/Tasks/KRAbilityTask_WaitTick.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "SubSystem/KREffectSubsystem.h"
#include "Data/DataAssets/KREffectDefinition.h"

const FName UKRGameplayAbility_RangeCharge::SECTION_START = FName("Start");
const FName UKRGameplayAbility_RangeCharge::SECTION_FAIL = FName("Fire");
const FName UKRGameplayAbility_RangeCharge::SECTION_SUCCESS = FName("Fire_Full");

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

	CurrentChargeMontage = GetMontageFromEquipment(0);
	if (!CurrentChargeMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bIsChargeSuccess = false;
	bIsFullyCharged = false;
	bIsCharging = true;
	CurrentChargeTime = 0.0f;

	ChargePlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("ChargePhase"),
		CurrentChargeMontage,
		1.0f,
		SECTION_START, 
		false 
	);

	if (ChargePlayTask)
	{
		ChargePlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnChargePhaseCompleted);
		ChargePlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnChargePhaseInterrupted);
		ChargePlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnChargePhaseInterrupted);
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
		return;
	}

	if (ChargeEffectTag.IsValid())
	{
		if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
		{
			if (USkeletalMeshComponent* Mesh = Character->GetMesh())
			{
				if (UWorld* World = GetWorld())
				{
					if (UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>())
					{
						UKREffectDefinition* EffectDef = EffectSubsystem->GetEffectDefinition(ChargeEffectTag);
						if (EffectDef)
						{
							const FKREffectSpawnSettings& SpawnSettings = EffectDef->SpawnSettings;
							FName SocketToUse = NAME_None;
							if (SpawnSettings.bAttachToParent)
							{
								SocketToUse = SpawnSettings.AttachSocketName;
							}

							if (SocketToUse != NAME_None)
							{
								EffectSubsystem->SpawnEffectAttached(
									ChargeEffectTag,
									Mesh,
									SocketToUse,
									SpawnSettings.LocationOffset,
									SpawnSettings.RotationOffset
								);
							}
							else
							{
								FTransform SpawnTransform = Character->GetActorTransform();
								SpawnTransform.SetLocation(SpawnTransform.GetLocation() + SpawnSettings.LocationOffset);
								EffectSubsystem->SpawnEffectByTag(ChargeEffectTag, SpawnTransform, Character);
							}
						}
					}
				}
			}
		}
	}
}

void UKRGameplayAbility_RangeCharge::OnChargeTick(float DeltaTime)
{
	if (!bIsCharging) return;

	CurrentChargeTime += DeltaTime;
	
	if (!bIsFullyCharged && CurrentChargeTime >= FullChargeTime)
	{
		bIsFullyCharged = true;
		bIsChargeSuccess = true;
		bIsCharging = false;

		if (ChargeTickTask)
		{
			ChargeTickTask->EndTask();
			ChargeTickTask = nullptr;
		}

		StartReleaseMontage(true);
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
	
	bIsChargeSuccess = false;
	
	StartReleaseMontage(false);
}

void UKRGameplayAbility_RangeCharge::StartReleaseMontage(bool bSuccess)
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char || !CurrentChargeMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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

	float FinalMultiplier = bSuccess ? DamageMulti_Success : DamageMulti_Fail;
	
	FRotator AimRot = GetFinalAimRotation(MaxRange);
	FireWeaponActor(AimRot, FinalMultiplier);
	
	const FName ReleaseSection = bSuccess ? SECTION_SUCCESS : SECTION_FAIL;
	
	ReleasePlayTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		   this,
		   FName("ReleasePhase"),
		   CurrentChargeMontage,
		   1.0f,
		   ReleaseSection,
		   true
		);

	if (ReleasePlayTask)
	{
		ReleasePlayTask->OnBlendOut.AddDynamic(this, &ThisClass::OnReleasePhaseCompleted);
		ReleasePlayTask->OnCompleted.AddDynamic(this, &ThisClass::OnReleasePhaseCompleted);
		ReleasePlayTask->OnInterrupted.AddDynamic(this, &ThisClass::OnReleasePhaseInterrupted);
		ReleasePlayTask->OnCancelled.AddDynamic(this, &ThisClass::OnReleasePhaseInterrupted);
		ReleasePlayTask->ReadyForActivation();
	}
	
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UKRGameplayAbility_RangeCharge::OnChargePhaseCompleted()
{
}

void UKRGameplayAbility_RangeCharge::OnChargePhaseInterrupted()
{
	if (!IsActive()) return;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGameplayAbility_RangeCharge::OnReleasePhaseCompleted()
{
	if (!IsActive()) return;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UKRGameplayAbility_RangeCharge::OnReleasePhaseInterrupted()
{
	if (!IsActive()) return;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UKRGameplayAbility_RangeCharge::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	bIsCharging = false;

	if (ChargeTickTask)
	{
		ChargeTickTask->EndTask();
		ChargeTickTask = nullptr;
	}

	if (ChargeEffectTag.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			if (UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>())
			{
				EffectSubsystem->StopEffectByTag(ChargeEffectTag);
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}