#include "KRMotionWarpingLibrary.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

bool UKRMotionWarpingLibrary::SetWarpTarget(AActor* Actor, FName TargetName, FVector Location, FRotator Rotation)
{
	UMotionWarpingComponent* MWComp = GetMotionWarpingComponent(Actor);
	if (!MWComp)
	{
		return false;
	}

	FMotionWarpingTarget Target;
	Target.Name = TargetName;
	Target.Location = Location;
	const FQuat Quat = FQuat(Rotation);
	Target.Rotation = Quat.Rotator();

	MWComp->AddOrUpdateWarpTarget(Target);
	return true;
}

bool UKRMotionWarpingLibrary::SetWarpTargetLocation(AActor* Actor, FName TargetName, FVector Location)
{
	if (!Actor)
	{
		return false;
	}

	return SetWarpTarget(Actor, TargetName, Location, Actor->GetActorRotation());
}

bool UKRMotionWarpingLibrary::SetWarpTargetRotation(AActor* Actor, FName TargetName, FRotator Rotation)
{
	if (!Actor)
	{
		return false;
	}

	return SetWarpTarget(Actor, TargetName, Actor->GetActorLocation(), Rotation);
}

bool UKRMotionWarpingLibrary::SetWarpTargetToActor(AActor* Actor, FName TargetName, AActor* TargetActor,
	float DistanceOffset, bool bFaceTarget)
{
	if (!Actor || !TargetActor)
	{
		return false;
	}

	FVector Direction = GetDirectionToActor(Actor, TargetActor, true);
	FVector TargetLocation = TargetActor->GetActorLocation() - (Direction * DistanceOffset);
	TargetLocation.Z = Actor->GetActorLocation().Z;

	FRotator TargetRotation = bFaceTarget
		? GetRotationFromDirection(Direction)
		: Actor->GetActorRotation();

	return SetWarpTarget(Actor, TargetName, TargetLocation, TargetRotation);
}

bool UKRMotionWarpingLibrary::SetWarpTargetToSocket(AActor* Actor, FName TargetName, AActor* TargetActor,
	FName SocketName, float DistanceOffset, bool bFaceTarget)
{
	if (!Actor || !TargetActor)
	{
		return false;
	}

	FVector SocketLocation = TargetActor->GetActorLocation();

	if (ACharacter* Character = Cast<ACharacter>(TargetActor))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			if (Mesh->DoesSocketExist(SocketName))
			{
				SocketLocation = Mesh->GetSocketLocation(SocketName);
			}
		}
	}

	FVector Direction = (SocketLocation - Actor->GetActorLocation()).GetSafeNormal2D();
	FVector TargetLocation = SocketLocation - (Direction * DistanceOffset);

	FRotator TargetRotation = bFaceTarget
		? GetRotationFromDirection(Direction)
		: Actor->GetActorRotation();

	return SetWarpTarget(Actor, TargetName, TargetLocation, TargetRotation);
}

bool UKRMotionWarpingLibrary::SetWarpTargetByDirection(AActor* Actor, FName TargetName, FVector Direction,
	float Distance, bool bFaceDirection)
{
	if (!Actor)
	{
		return false;
	}

	FVector NormalizedDir = Direction.GetSafeNormal2D();
	FVector TargetLocation = Actor->GetActorLocation() + (NormalizedDir * Distance);

	FRotator TargetRotation = bFaceDirection
		? GetRotationFromDirection(NormalizedDir)
		: Actor->GetActorRotation();

	return SetWarpTarget(Actor, TargetName, TargetLocation, TargetRotation);
}

bool UKRMotionWarpingLibrary::SetKnockbackWarpTarget(AActor* Victim, FName TargetName, AActor* Attacker,
	float KnockbackDistance, bool bFaceAttacker)
{
	if (!Victim || !Attacker)
	{
		return false;
	}

	FVector KnockbackDirection = GetDirectionToActor(Attacker, Victim, true);
	FVector TargetLocation = Victim->GetActorLocation() + (KnockbackDirection * KnockbackDistance);

	FRotator TargetRotation = Victim->GetActorRotation();
	if (bFaceAttacker)
	{
		FVector ToAttacker = GetDirectionToActor(Victim, Attacker, true);
		TargetRotation = GetRotationFromDirection(ToAttacker);
	}

	return SetWarpTarget(Victim, TargetName, TargetLocation, TargetRotation);
}

bool UKRMotionWarpingLibrary::RemoveWarpTarget(AActor* Actor, FName TargetName)
{
	UMotionWarpingComponent* MWComp = GetMotionWarpingComponent(Actor);
	if (!MWComp)
	{
		return false;
	}

	MWComp->RemoveWarpTarget(TargetName);
	return true;
}

bool UKRMotionWarpingLibrary::RemoveAllWarpTargets(AActor* Actor)
{
	UMotionWarpingComponent* MWComp = GetMotionWarpingComponent(Actor);
	if (!MWComp)
	{
		return false;
	}

	MWComp->RemoveAllWarpTargets();
	return true;
}

UMotionWarpingComponent* UKRMotionWarpingLibrary::GetMotionWarpingComponent(AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	return Actor->FindComponentByClass<UMotionWarpingComponent>();
}

FVector UKRMotionWarpingLibrary::GetDirectionToActor(AActor* FromActor, AActor* ToActor, bool bIgnoreZ)
{
	if (!FromActor || !ToActor)
	{
		return FVector::ForwardVector;
	}

	FVector Direction = ToActor->GetActorLocation() - FromActor->GetActorLocation();

	if (bIgnoreZ)
	{
		Direction.Z = 0.f;
	}

	return Direction.GetSafeNormal();
}

FRotator UKRMotionWarpingLibrary::GetRotationFromDirection(FVector Direction)
{
	if (Direction.IsNearlyZero())
	{
		return FRotator::ZeroRotator;
	}

	return Direction.Rotation();
}
