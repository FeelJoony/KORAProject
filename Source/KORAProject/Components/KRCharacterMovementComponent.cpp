#include "Components/KRCharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
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