#include "GAS/Abilities/HeroAbilities/KRGA_LockOn.h"
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
#include "GameplayTag/KRStateTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/KREnemyCharacter.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

UKRGA_LockOn::UKRGA_LockOn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

AActor* UKRGA_LockOn::GetLockedTargetFor(AActor* Actor)
{
	if (!Actor) return nullptr;

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	if (!ASC) return nullptr;

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.IsActive()) continue;

		if (UKRGA_LockOn* LockOnAbility = Cast<UKRGA_LockOn>(Spec.GetPrimaryInstance()))
		{
			return LockOnAbility->GetLockedTarget();
		}
	}

	return nullptr;
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
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->AddLooseGameplayTag(KRTAG_STATE_ACTING_LOCKON);
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
	CurrentSocketName = NAME_None;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RemoveLooseGameplayTag(KRTAG_STATE_ACTING_LOCKON);
	}
	
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

void UKRGA_LockOn::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UKRGA_LockOn::FindTarget()
{
	TArray<AActor*> Candidates;
	GetAvailableTargets(Candidates);

	AActor* BestTarget = nullptr;
	FName BestSocket = NAME_None;
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

		TArray<FName> Sockets = GetTargetSockets(Candidate);

		for (const FName& SocketName : Sockets)
		{
			FVector TargetLoc = GetTargetLocation(Candidate, SocketName);
			FVector DirToTarget = (TargetLoc - CameraLoc).GetSafeNormal();
			
			float Dot = FVector::DotProduct(CameraDir, DirToTarget);
			float Distance = FVector::Dist(CameraLoc, TargetLoc);

			if (Dot > 0.5f)
			{
				float Score = (LockOnRadius - Distance) + (Dot * 1000.f);
				if (Score > BestScore)
				{
					BestScore = Score;
					BestTarget = Candidate;
					BestSocket = SocketName;
				}
			}
		}
	}
	CurrentTarget = BestTarget;
	CurrentSocketName = BestSocket;
}

void UKRGA_LockOn::ResetCamera()
{
	AKRPlayerController* PC = GetKRPlayerControllerFromActorInfo();
	AKRHeroCharacter* Hero = Cast<AKRHeroCharacter>(GetAvatarActorFromActorInfo());

	if (!PC || !Hero)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	CameraResetStartRot = PC->GetControlRotation();
	CameraResetTargetRot = Hero->GetActorRotation();
	CameraResetTargetRot.Pitch = 0.f;
	CameraResetTargetRot.Roll  = 0.f;
	CameraResetElapsedTime = 0.f;
	
	if (CameraResetDuration <= KINDA_SMALL_NUMBER)
	{
		CameraResetDuration = 0.2f; 
	}
	
	if (UKRAbilityTask_WaitTick* TickTask = UKRAbilityTask_WaitTick::WaitTick(this, FName("CameraResetTick")))
	{
		TickTask->OnTick.AddDynamic(this, &ThisClass::OnResetCameraTick);
		TickTask->ReadyForActivation();
	}
	else
	{
		PC->SetControlRotation(CameraResetTargetRot);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UKRGA_LockOn::OnResetCameraTick(float DeltaTime)
{
	AKRPlayerController* PC = GetKRPlayerControllerFromActorInfo();
	if (!PC || !IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	CameraResetElapsedTime += DeltaTime;
	
	float Alpha = FMath::Clamp(CameraResetElapsedTime / CameraResetDuration, 0.f, 1.f);
	
	FRotator NewRot = FMath::Lerp(CameraResetStartRot, CameraResetTargetRot, Alpha);
	
	PC->SetControlRotation(NewRot);
	
	if (Alpha >= 1.f)
	{
		PC->SetControlRotation(CameraResetTargetRot);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

FVector UKRGA_LockOn::GetTargetLocation(AActor* Target, FName SocketName) const
{
	if (!Target) return FVector::ZeroVector;

	if (!SocketName.IsNone())
	{
		if (ACharacter* Char = Cast<ACharacter>(Target))
		{
			if (Char->GetMesh() && Char->GetMesh()->DoesSocketExist(SocketName))
			{
				return Char->GetMesh()->GetSocketLocation(SocketName);
			}
		}
	}

	FVector OutLoc = Target->GetActorLocation();
	if (ACharacter* Char = Cast<ACharacter>(Target))
	{
		OutLoc.Z -= Char->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 0.2f;
	}
	return OutLoc;
}

TArray<FName> UKRGA_LockOn::GetTargetSockets(AActor* Target) const
{
	if (AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(Target))
	{
		const TArray<FName>& Sockets = Enemy->GetLockOnSockets();
		if (Sockets.Num() > 0)
		{
			return Sockets;
		}
	}

	return {FName("LockOn_Spine")};
}

void UKRGA_LockOn::OnTick(float DeltaTime)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (!IsTargetValid(CurrentTarget) || AvatarActor->GetDistanceTo(CurrentTarget) > LockOnBreakDistance)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (!GetAbilitySystemComponentFromActorInfo()->HasMatchingGameplayTag(KRTAG_STATE_ACTING_UNINTERRUPTIBLE))
	{
		UpdateTargetRotation(DeltaTime);
	}

	if (LockOnWidgetInstance && GetKRPlayerControllerFromActorInfo())
	{
		FVector TargetLoc = GetTargetLocation(CurrentTarget, CurrentSocketName);
		FVector2D ScreenPos;
		if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetKRPlayerControllerFromActorInfo(), TargetLoc, ScreenPos, true))
		{
			LockOnWidgetInstance->SetPositionInViewport(ScreenPos, false);
			LockOnWidgetInstance->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
		}
	}
	
	CheckForTargetSwitch();
}

void UKRGA_LockOn::UpdateTargetRotation(float DeltaTime)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor)
	{
		return;
	}

	FVector TargetLoc = GetTargetLocation(CurrentTarget, CurrentSocketName);

	if (AKRPlayerController* PC = GetKRPlayerControllerFromActorInfo())
	{
		FVector CameraLoc;
		FRotator CameraRot;
		PC->GetPlayerViewPoint(CameraLoc, CameraRot);

		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(CameraLoc, TargetLoc);
		
		if (bAdjustPitchBasedOnDistance)
		{
			float DistanceToTarget = FVector::Dist(AvatarActor->GetActorLocation(), TargetLoc);
			float PitchOffset = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1000.f), FVector2D(10.f, 0.f), DistanceToTarget);

			LookAtRot.Pitch += PitchOffset;
		}

		FRotator CurrentRot = PC->GetControlRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, RotationInterpSpeed);

		// 거리 기반 Pitch 제한
		float MinPitch = -60.f;
		if (bLimitPitchAtCloseRange)
		{
			const float DistanceToTarget = FVector::Dist(AvatarActor->GetActorLocation(), TargetLoc);
			if (DistanceToTarget < CloseRangeThreshold)
			{
				// 가까울수록 Pitch를 덜 아래로 내려가게 제한
				const float DistRatio = DistanceToTarget / CloseRangeThreshold;
				MinPitch = FMath::Lerp(CloseRangePitchMin, -60.f, DistRatio);
			}
		}

		NewRot.Pitch = FMath::ClampAngle(NewRot.Pitch, MinPitch, 30.f);

		PC->SetControlRotation(NewRot);
	}
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
	InputDirection.Y *= -1.f;
	
	if (!CurrentTarget) return;

	TArray<AActor*> Candidates;
	GetAvailableTargets(Candidates);

	AActor* BestNewTarget = nullptr;
	FName BestNewSocket = NAME_None;
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

	FVector CurrentTargetWorldLoc = GetTargetLocation(CurrentTarget, CurrentSocketName);
	FVector CurrentTargetLocal = CameraTransform.InverseTransformPosition(CurrentTargetWorldLoc);

	for (AActor* Candidate : Candidates)
	{
		if (!IsTargetValid(Candidate)) continue;
		
		TArray<FName> Sockets = GetTargetSockets(Candidate);

		for (const FName& SocketName : Sockets)
		{
			if (Candidate == CurrentTarget && SocketName == CurrentSocketName) continue;

			FVector CandWorldLoc = GetTargetLocation(Candidate, SocketName);
			FVector CandidateLocal = CameraTransform.InverseTransformPosition(CandWorldLoc);

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
					BestNewSocket = SocketName;
				}
			}
		}
	}

	if (BestNewTarget)
	{
		CurrentTarget = BestNewTarget;
		CurrentSocketName = BestNewSocket;
	}
}

bool UKRGA_LockOn::IsTargetValid(AActor* TargetActor) const
{
	if (!TargetActor || !IsValid(TargetActor)) return false;

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		if (TargetASC->HasMatchingGameplayTag(KRTAG_ENEMY_AISTATE_DEAD))
		{
			return false;
		}
	}
	
	float Distance = GetAvatarActorFromActorInfo()->GetDistanceTo(TargetActor);
	if (Distance > LockOnBreakDistance) return false;

	TArray<FHitResult> Hits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());
	Params.AddIgnoredActor(TargetActor);

	FVector Start = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector End = GetTargetLocation(TargetActor, CurrentSocketName);

	bool bHit = GetWorld()->LineTraceMultiByChannel(Hits, Start, End, ECC_Visibility, Params);
	if (bHit)
	{
		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			if (HitActor->IsA(APawn::StaticClass())) continue;

			return false;
		}
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