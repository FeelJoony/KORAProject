#include "GAS/Abilities/HeroAbilities/KRGA_HeroDash.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

void UKRGA_HeroDash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor);
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	//input
	FVector LocalInputVector = Character->GetLastMovementInputVector();
    LocalInputVector.Z = 0.0f;
    
    const FRotator ControlRotation = Character->GetController()->GetControlRotation();
    const FRotator YawRotation(0, ControlRotation.Yaw, 0);
    
    FVector WorldDirection = UKismetMathLibrary::GreaterGreater_VectorRotator(LocalInputVector, YawRotation);
    WorldDirection.Normalize(); 

	LaunchCharacter(WorldDirection, Character);
}

void UKRGA_HeroDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HeroDash::LaunchCharacter(const FVector& InWorldDirection, ACharacter* Character)
{
	EDashDirection DashEnum = GetDashDirection(InWorldDirection);
    
	UAnimMontage* SelectedMontage = nullptr; 
	if (DashMontages.Contains(DashEnum))
	{
		SelectedMontage = DashMontages[DashEnum];
	}
	if (SelectedMontage)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("DashMontageTask"), SelectedMontage
		);
		PlayMontageTask->OnCompleted.AddDynamic(this, &UKRGA_HeroDash::OnAbilityEnd);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_HeroDash::OnAbilityEnd);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UKRGA_HeroDash::OnAbilityEnd);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_HeroDash::OnAbilityEnd);
		PlayMontageTask->ReadyForActivation();

		//Launch
		FVector LaunchVelocity = InWorldDirection * DashSpeed;
		Character->LaunchCharacter(LaunchVelocity, true, true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Dash Montage not found for direction: %d"), (int32)DashEnum);
		OnAbilityEnd();
	}
}

void UKRGA_HeroDash::OnAbilityEnd()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

EDashDirection UKRGA_HeroDash::GetDashDirection(const FVector& InputVector) const
{
	if (InputVector.IsNearlyZero())
	{
		return EDashDirection::Forward;
	}

	float Angle = FMath::Atan2(InputVector.Y, InputVector.X);
	
	Angle = FMath::RadiansToDegrees(Angle);
	
	if (Angle < 0.f)
	{
		Angle += 360.f;
	}

	float SectorAngle = FMath::Fmod(Angle + 22.5f + 360.f, 360.f);
    
	int32 DirectionIndex = FMath::FloorToInt(SectorAngle / 45.f);
	
	return static_cast<EDashDirection>(DirectionIndex);
}


