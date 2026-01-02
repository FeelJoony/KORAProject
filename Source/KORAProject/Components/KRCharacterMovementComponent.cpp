#include "Components/KRCharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Character.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KREventTag.h"
#include "Interaction/KRLadderActor.h"

float UKRCharacterMovementComponent::GetMaxSpeed() const
{
	if (MovementMode == MOVE_Custom && CustomMovementMode == (uint8)EKRMovementMode::Ladder)
	{
		return MaxLadderSpeed;
	}
	
	float MaxSpeed = Super::GetMaxSpeed();

	const ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar)
	{
		return MaxSpeed;
	}
	
	if (!CachedASC.IsValid())
	{
		if (UAbilitySystemComponent* BaseASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerChar))
		{
			CachedASC = Cast<UKRAbilitySystemComponent>(BaseASC);
		}
	}
	
	if (CachedASC.IsValid())
	{
		if (CachedASC->HasMatchingGameplayTag(KRTAG_STATE_RESTRICT_INPUT))
		{
			return 0.f;
		}
	}

	return MaxSpeed;
}

void UKRCharacterMovementComponent::StartClimbingLadder(const AKRLadderActor* NewLadder)
{
	if (!NewLadder) return;

	CurrentLadder = NewLadder;
	SetMovementMode(MOVE_Custom, (uint8)EKRMovementMode::Ladder);
	bIsLadderMounting = true; 
	
	if (CharacterOwner)
	{
		bOrientRotationToMovement = false; 
		CharacterOwner->bUseControllerRotationYaw = false;

		StopMovementImmediately();
	}
}

void UKRCharacterMovementComponent::StopClimbingLadder()
{
	CurrentLadder = nullptr;
	SetMovementMode(MOVE_Walking);
	bIsLadderMounting = false;

	bOrientRotationToMovement = true;
	if (CharacterOwner)
	{
		CharacterOwner->bUseControllerRotationYaw = false;
	}

	StopMovementImmediately();
}

void UKRCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* BaseASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()); 
	CachedASC = Cast<UKRAbilitySystemComponent>(BaseASC);
}

void UKRCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (CustomMovementMode == (uint8)EKRMovementMode::Ladder)
	{
		PhysLadder(deltaTime, Iterations);
	}
	
	Super::PhysCustom(deltaTime, Iterations);
}

void UKRCharacterMovementComponent::PhysLadder(float deltaTime, int32 Iterations)
{
	if (!CurrentLadder)
	{
		StopClimbingLadder();
		return;
	}

	if (bIsLadderMounting)
	{
		if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
		{
			ApplyRootMotionToVelocity(deltaTime);
			
			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Velocity * deltaTime, CharacterOwner->GetActorRotation(), false, Hit);
		}
		
		return;
	}
	
	float ClimbDirection = GetClimbDirection();

	Velocity = FVector::UpVector * ClimbDirection * GetMaxSpeed();
	
	FVector Delta = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Delta, CharacterOwner->GetActorRotation(), true, Hit);
	
	SnapToLadderLocation();
	
	if (CharacterOwner)
	{
		FRotator TargetRotation = CurrentLadder->GetActorForwardVector().Rotation();
		TargetRotation.Yaw += CurrentLadder->LadderYawOffset + 180.f;
        
		CharacterOwner->SetActorRotation(TargetRotation);
	}

	CheckLadderExit(ClimbDirection);
}

float UKRCharacterMovementComponent::GetClimbDirection() const
{
	const FVector InputVector = GetLastInputVector();
	
	if (InputVector.IsNearlyZero())
	{
		return 0.f;
	}

	const FVector ControlForward = CharacterOwner->GetControlRotation().Vector();
	const float Dot = FVector::DotProduct(InputVector.GetSafeNormal(), ControlForward);

	if (Dot > 0.1f) return 1.f;
	if (Dot < -0.1f) return -1.f;
	
	return 0.f;
}

void UKRCharacterMovementComponent::SnapToLadderLocation()
{
	if (!CurrentLadder || !CharacterOwner) return;

	FVector CurrentLoc = CharacterOwner->GetActorLocation();

	FTransform SnapTrans = CurrentLadder->GetSnapTransform(CurrentLoc);

	FVector CorrectedLoc = SnapTrans.GetLocation();
	CorrectedLoc.Z = CurrentLoc.Z; 
	
	if (!CurrentLoc.Equals(CorrectedLoc, 1.f))
	{
		FHitResult SnapHit(1.f);
		SafeMoveUpdatedComponent(CorrectedLoc - CurrentLoc, CharacterOwner->GetActorRotation(), true, SnapHit);
	}
}

void UKRCharacterMovementComponent::CheckLadderExit(float ClimbDirection)
{
	if (!CurrentLadder || !CharacterOwner) return;

	const float CharacterZ = CharacterOwner->GetActorLocation().Z;
	const float TopZ = CurrentLadder->GetTopLocation().Z;
	const float BottomZ = CurrentLadder->GetBottomLocation().Z;
	const float HalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const float TopTriggerOffset = 45.0f; 

	if (ClimbDirection > 0.f && (CharacterZ >= TopZ - TopTriggerOffset)) 
	{
		FGameplayEventData Payload;
		Payload.EventTag = KRTAG_EVENT_LADDER_TOP;
		Payload.Instigator = CharacterOwner;
		Payload.Target = CharacterOwner;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(CharacterOwner, KRTAG_EVENT_LADDER_TOP, Payload);
		
		Velocity = FVector::ZeroVector;
	}

	else if (ClimbDirection < 0.f && (CharacterZ - HalfHeight <= BottomZ))
	{
		FGameplayEventData Payload;
		Payload.EventTag = KRTAG_EVENT_LADDER_BOTTOM;
		Payload.Instigator = CharacterOwner;
		Payload.Target = CharacterOwner;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(CharacterOwner, KRTAG_EVENT_LADDER_BOTTOM, Payload);

		Velocity = FVector::ZeroVector;
	}
}

void UKRCharacterMovementComponent::SetLadderMounting(bool bMounting)
{
	bIsLadderMounting = bMounting;
}

void UKRCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bEnableCharacterSeparation && CharacterOwner)
	{
		ApplyCharacterSeparation(DeltaTime);
	}
}

void UKRCharacterMovementComponent::SetCharacterSeparationEnabled(bool bEnabled)
{
	bEnableCharacterSeparation = bEnabled;
}

void UKRCharacterMovementComponent::ApplyCharacterSeparation(float DeltaTime)
{
	if (!CharacterOwner) return;

	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	if (!Capsule) return;

	const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	const float DetectionRadius = CapsuleRadius + SeparationDetectionRadius;
	const FVector OwnerLocation = CharacterOwner->GetActorLocation();

	// 주변 캐릭터 탐지
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(CharacterOwner);

	const bool bHasOverlaps = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		OwnerLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(DetectionRadius + 100.0f),  // 여유있게 탐지
		QueryParams
	);

	if (!bHasOverlaps) return;

	FVector TotalSeparation = FVector::ZeroVector;
	int32 SeparationCount = 0;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		ACharacter* OtherChar = Cast<ACharacter>(Overlap.GetActor());
		if (!OtherChar) continue;

		UCapsuleComponent* OtherCapsule = OtherChar->GetCapsuleComponent();
		if (!OtherCapsule) continue;

		FVector ToOther = OtherChar->GetActorLocation() - OwnerLocation;
		ToOther.Z = 0.0f;  // 수평 방향만 고려

		const float Distance = ToOther.Size();
		if (Distance < KINDA_SMALL_NUMBER) continue;

		const float OtherRadius = OtherCapsule->GetScaledCapsuleRadius();
		const float MinRequiredDist = CapsuleRadius + OtherRadius + MinSeparationBuffer;

		if (Distance < MinRequiredDist)
		{
			// 분리 방향 (다른 캐릭터에서 멀어지는 방향)
			const FVector SeparationDir = -ToOther.GetSafeNormal();

			// 거리에 반비례하는 분리력 (가까울수록 강함)
			// 0 = 완전히 겹침, 1 = 최소 거리에서 접촉
			const float OverlapRatio = FMath::Clamp(1.0f - (Distance / MinRequiredDist), 0.0f, 1.0f);

			// 거리 기반 가중치 (더 가까운 캐릭터에 더 강한 반응)
			TotalSeparation += SeparationDir * OverlapRatio;
			SeparationCount++;
		}
	}

	if (SeparationCount > 0 && !TotalSeparation.IsNearlyZero())
	{
		TotalSeparation.Normalize();

		// 공격 중이면 분리력 감소 (타겟에 가까이 유지)
		float FinalStrength = SeparationStrength;
		if (IsPerformingAttack())
		{
			FinalStrength *= SeparationMultiplierWhileAttacking;
		}

		// AddInputVector로 부드럽게 분리 (물리 충돌 없이 자연스러운 이동)
		AddInputVector(TotalSeparation * FinalStrength * DeltaTime);
	}
}

bool UKRCharacterMovementComponent::IsPerformingAttack() const
{
	if (!CachedASC.IsValid()) return false;

	return CachedASC->HasMatchingGameplayTag(KRTAG_STATE_ACTING_ATTACKING);
}