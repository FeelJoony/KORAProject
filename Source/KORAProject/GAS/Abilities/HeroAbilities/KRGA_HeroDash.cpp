#include "GAS/Abilities/HeroAbilities/KRGA_HeroDash.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/KRHeroCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/KRHeroComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/KRAbilitySystemComponent.h"

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

	UKRHeroComponent* HeroComponent = Character->FindComponentByClass<UKRHeroComponent>();
	if (!HeroComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("HeroComponent not found"));
	}
	const FVector2D MoveInput = HeroComponent->GetLastMoveInput();
	//UE_LOG(LogTemp,Warning,TEXT("[GA_Dash] MoveInput.X : %f, MoveInput.Y : %f"),MoveInput.X, MoveInput.Y)
	
	LaunchCharacter(MoveInput);
}

void UKRGA_HeroDash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_HeroDash::LaunchCharacter(const FVector2D& Input)
{
	UKRAbilitySystemComponent* ASC = GetKRAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	EDashDirection DashEnum;
	if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("State.Acting.LockOn"))))
	{
		DashEnum = SelectDashDirectionByInput(Input);
	}
	else
	{
		DashEnum = EDashDirection::Default;
	}
	
	UAnimMontage* SelectedMontage = nullptr;
	if (DashMontages.Contains(DashEnum))
	{
		SelectedMontage = DashMontages[DashEnum];
	}
	if (SelectedMontage != nullptr)
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, TEXT("DashMontageTask"), SelectedMontage
		);
		PlayMontageTask->OnCompleted.AddDynamic(this, &UKRGA_HeroDash::OnAbilityEnd);
		PlayMontageTask->OnInterrupted.AddDynamic(this, &UKRGA_HeroDash::OnAbilityEnd);
		PlayMontageTask->OnCancelled.AddDynamic(this, &UKRGA_HeroDash::OnAbilityEnd);
		PlayMontageTask->OnBlendOut.AddDynamic(this, &UKRGA_HeroDash::OnAbilityEnd);
		PlayMontageTask->ReadyForActivation();
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

EDashDirection UKRGA_HeroDash::SelectDashDirectionByInput(const FVector2D& Input)
{
	FVector CurrentInput = GetKRCharacterFromActorInfo()->GetCharacterMovement()->GetLastInputVector();
	if (FMath::IsNearlyZero(CurrentInput.X) && FMath::IsNearlyZero(CurrentInput.Y))
	{
		UE_LOG(LogTemp, Warning, TEXT("None Input"));
		return EDashDirection::Backward;
	}
	
	float X = Input.X;	
	float Y = Input.Y;

	if (X > 0.3f)
	{
		if (Y >= 0.3f)					  return EDashDirection::ForwardRight;
		if (Y <= -0.3f)					  return EDashDirection::BackwardRight;
		return EDashDirection::Right;
	}

	if (X < -0.3f)
	{
		if (Y >= 0.3f)					  return EDashDirection::ForwardLeft;
		if (Y <= -0.3f)					  return EDashDirection::BackwardLeft;
		return EDashDirection::Left;
	}

	if (Y > 0.3f)       return EDashDirection::Forward;
	if (Y < -0.3f)       return EDashDirection::Backward;
	return EDashDirection::Backward;
}

FVector UKRGA_HeroDash::CalculateDirection(EDashDirection DashEnum)
{
	AKRBaseCharacter* Character = GetKRCharacterFromActorInfo();
	if (!Character) return FVector::BackwardVector;
	
	FRotator ControlRot = Character->GetControlRotation();
    FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
	FVector LocalDir = FVector::BackwardVector;
	
	switch (DashEnum)
	{
		case EDashDirection::Forward:         LocalDir = FVector(0, 1, 0); break;
		case EDashDirection::ForwardRight:    LocalDir = FVector(1, 1, 0); break;
		case EDashDirection::Right:           LocalDir = FVector(1, 0, 0); break;
		case EDashDirection::BackwardRight:   LocalDir = FVector(1, -1, 0); break;
		case EDashDirection::Backward:        LocalDir = FVector(0, -1, 0); break;
		case EDashDirection::BackwardLeft:    LocalDir = FVector(-1, -1, 0); break;
		case EDashDirection::Left:            LocalDir = FVector(-1, 0, 0); break;
		case EDashDirection::ForwardLeft:     LocalDir = FVector(-1, 1, 0); break;

		default: break;
	}

	FVector WorldDir = LocalDir.GetSafeNormal();
	WorldDir = UKismetMathLibrary::GreaterGreater_VectorRotator(WorldDir, YawRot);

	return WorldDir;
}
