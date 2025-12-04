#include "KRGA_LockOn.h"
#include "GAS/Abilities/Tasks/KRAbilityTask_WaitTick.h"
#include "Characters/KRHeroCharacter.h"
#include "Components/KRCameraComponent.h"
#include "Camera/KRCameraMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/KRPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/KRHeroComponent.h"
#include "GameplayTag/KREnemyTag.h"

UKRGA_LockOn::UKRGA_LockOn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKRGA_LockOn::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (CurrentTarget)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true ,false);
		return;
	}
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FindTarget();

	if (!CurrentTarget)
	{
		ResetCamera();
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (AKRHeroCharacter* Hero = Cast<AKRHeroCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UKRCameraComponent* CameraComp = UKRCameraComponent::FindCameraComponent(Hero))
		{
			if (LockOnCameraModeClass)
			{
				CameraComp->PushCameraMode(LockOnCameraModeClass);
			}
		}
		Hero->SetMovementMode_Strafe(true);
	}

	if (LockOnWidgetClass && GetKRPlayerControllerFromActorInfo())
	{
		LockOnWidgetInstance = CreateWidget<UUserWidget>(GetKRPlayerControllerFromActorInfo(), LockOnWidgetClass);
		if (LockOnWidgetInstance)
		{
			LockOnWidgetInstance->AddToViewport();
		}
	}

	UKRAbilityTask_WaitTick* TickTask = UKRAbilityTask_WaitTick::WaitTick(this, FName("LockOnTick"));
	if (TickTask)
	{
		TickTask->OnTick.AddDynamic(this, &ThisClass::OnTick);
		TickTask->ReadyForActivation();
	}
}

void UKRGA_LockOn::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (LockOnWidgetInstance)
	{
		LockOnWidgetInstance->RemoveFromParent();
		LockOnWidgetInstance = nullptr;
	}

	CurrentTarget = nullptr;

	if (AKRHeroCharacter* Hero = Cast<AKRHeroCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UKRCameraComponent* CameraComp = UKRCameraComponent::FindCameraComponent(Hero))
		{
			CameraComp->RemoveCameraMode(LockOnCameraModeClass);
		}
		Hero->SetMovementMode_Strafe(false);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UKRGA_LockOn::FindTarget()
{
	TArray<AActor*> Candidates;
	GetAvailableTargets(Candidates);

	AActor* BestTarget = nullptr;
	float BestScore = -1.f;

	AKRPlayerController* PC = GetKRPlayerControllerFromActorInfo();
	if (!PC) return;

	FVector CameraLoc;
	FRotator CameraRot;
	PC->GetPlayerViewPoint(CameraLoc, CameraRot);
	FVector CameraDir = CameraRot.Vector();

	for (AActor* Candidate : Candidates)
	{
		if (!IsTargetValid(Candidate)) continue;

		FVector DirToTarget = (Candidate->GetActorLocation() - CameraLoc).GetSafeNormal();
		float Dot = FVector::DotProduct(CameraDir, DirToTarget);

		float Distance = GetAvatarActorFromActorInfo()->GetDistanceTo(Candidate);

		if (Dot > 0.5f)
		{
			float Score = (LockOnRadius - Distance) + (Dot * 1000.f);
			if (Score > BestScore)
			{
				BestScore = Score;
				BestTarget = Candidate;
			}
		}
	}
	CurrentTarget = BestTarget;
}

void UKRGA_LockOn::ResetCamera()
{
	if (AKRHeroCharacter* Hero = Cast<AKRHeroCharacter>(GetAvatarActorFromActorInfo()))
	{
		GetWorld()->GetTimerManager().SetTimer(CameraResetTimerHandle, this, &ThisClass::UpdateCameraReset, 0.01f, true);
	}
}

void UKRGA_LockOn::UpdateCameraReset()
{
	AKRPlayerController* PC = GetKRPlayerControllerFromActorInfo();
	AActor* Actor = GetAvatarActorFromActorInfo();

	if (!PC || !Actor)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraResetTimerHandle);
		return;
	}

	FRotator CurrentRot = PC->GetControlRotation();
	FRotator TargetRot = Actor->GetActorRotation();

	TargetRot.Pitch = 0.f;

	FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, 0.01f, CameraResetInterpSpeed);
	PC->SetControlRotation(NewRot);

	if (FMath::IsNearlyEqual(NewRot.Yaw, TargetRot.Yaw, 1.f))
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraResetTimerHandle);
	}
}

void UKRGA_LockOn::OnTick(float DeltaTime)
{
	if (!IsTargetValid(CurrentTarget) || GetAvatarActorFromActorInfo()->GetDistanceTo(CurrentTarget) > LockOnBreakDistance)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (AKRPlayerController* PC = GetKRPlayerControllerFromActorInfo())
	{
		FVector MyLoc = GetAvatarActorFromActorInfo()->GetActorLocation();
		FVector TargetLoc = CurrentTarget->GetActorLocation();

		if (ACharacter* TargetChar = Cast<ACharacter>(CurrentTarget))
		{
			TargetLoc.Z -= TargetChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.2f;
		}

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);
		FRotator CurrentRot = PC->GetControlRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, RotationInterpSpeed);

		NewRot.Pitch = FMath::ClampAngle(NewRot.Pitch, -45.f, 45.f);

		PC->SetControlRotation(NewRot);
	}

	if (LockOnWidgetInstance && GetKRPlayerControllerFromActorInfo())
	{
		FVector2D ScreenPos;
		FVector WidgetWorldLoc = CurrentTarget->GetActorLocation();

		if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetKRPlayerControllerFromActorInfo(), WidgetWorldLoc, ScreenPos, true))
		{
			LockOnWidgetInstance->SetPositionInViewport(ScreenPos);
		}
	}
	
	CheckForTargetSwitch();
}

void UKRGA_LockOn::CheckForTargetSwitch()
{
	UKRHeroComponent* HeroComp = nullptr;
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		HeroComp = Avatar->FindComponentByClass<UKRHeroComponent>();
	}

	if (!HeroComp) return;

	FVector2D LookInput = HeroComp->GetLastLookInput();
	float InputMagnitude = LookInput.Size();
	
	bool bIsMouseInput = InputMagnitude > 1.1f;
	float Threshold = bIsMouseInput ? TargetSwitchMouseThreshold : TargetSwitchStickThreshold;
	
	if (InputMagnitude > Threshold)
	{
		if (bCanSwitchTarget)
		{
			FVector2D SwitchDir = LookInput.GetSafeNormal();
			SwitchTarget(SwitchDir);
			bCanSwitchTarget = false;
		}
	}
	else
	{
		if (InputMagnitude < (Threshold * 0.5f))
		{
			bCanSwitchTarget = true;
		}
	}
}

void UKRGA_LockOn::SwitchTarget(FVector2D InputDirection)
{
	if (!CurrentTarget) return;

	TArray<AActor*> Candidates;
	GetAvailableTargets(Candidates);

	AActor* BestNewTarget = nullptr;
	float BestScore = -1.f;

	FVector CameraLoc;
	FRotator CameraRot;
	if (AKRPlayerController* PC = GetKRPlayerControllerFromActorInfo())
	{
		PC->GetPlayerViewPoint(CameraLoc, CameraRot);
	}
	else
	{
		return;
	}

	FTransform CameraTransform(CameraRot, CameraLoc);

	FVector CurrentTargetLocal = CameraTransform.InverseTransformPosition(CurrentTarget->GetActorLocation());

	for (AActor* Candidate : Candidates)
	{
		if (Candidate == CurrentTarget || !IsTargetValid(Candidate)) continue;

		FVector CandidateLocal = CameraTransform.InverseTransformPosition(Candidate->GetActorLocation());
		FVector RelativeDelta = CandidateLocal - CurrentTargetLocal;
		FVector2D ScreenDirToCandidate = FVector2D(RelativeDelta.Y, RelativeDelta.Z).GetSafeNormal();
		
		float Dot = FVector2D::DotProduct(InputDirection, ScreenDirToCandidate);
		
		if (Dot > 0.5f)
		{
			float Distance = RelativeDelta.Size();
			float NormalizedDistScore = FMath::Clamp(1.0f - (Distance / LockOnRadius), 0.0f, 1.0f);
			float TotalScore = (Dot * DirectionScoreWeight) + (NormalizedDistScore * DistanceScoreWeight);

			if (TotalScore > BestScore)
			{
				BestScore = TotalScore;
				BestNewTarget = Candidate;
			}
		}
	}
	if (BestNewTarget)
	{
		CurrentTarget = BestNewTarget;
	}
}



bool UKRGA_LockOn::IsTargetValid(AActor* TargetActor) const
{
	if (!TargetActor || !IsValid(TargetActor)) return false;

	IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(TargetActor);
	if (TagInterface && TagInterface->HasMatchingGameplayTag(KRTAG_ENEMY_AISTATE_DEAD))
	{
		return false;
	}
	
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());
	Params.AddIgnoredActor(TargetActor);

	FVector Start = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector End = TargetActor->GetActorLocation();

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
	if (bHit)
	{
		return false;
	}

	return true;
}

void UKRGA_LockOn::GetAvailableTargets(TArray<AActor*>& OutActors)
{
	FVector Start = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetAvatarActorFromActorInfo());

	UKismetSystemLibrary::SphereOverlapActors(
		this,
		Start,
		LockOnRadius,
		TargetTraceChannels,
		APawn::StaticClass(),
		IgnoredActors,
		OutActors
		);
}
