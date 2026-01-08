#include "KRHitReactionTypes.h"
#include "GameFramework/Actor.h"

EKRHitDirection UKRHitReactionLibrary::CalculateHitDirection(AActor* Victim, AActor* Instigator)
{
	if (!Victim || !Instigator)
	{
		return EKRHitDirection::Front;
	}

	FVector HitDir = (Victim->GetActorLocation() - Instigator->GetActorLocation()).GetSafeNormal();
	return CalculateHitDirectionFromVector(Victim, HitDir);
}

EKRHitDirection UKRHitReactionLibrary::CalculateHitDirectionFromVector(AActor* Victim, const FVector& HitDirection)
{
	if (!Victim)
	{
		return EKRHitDirection::Front;
	}

	FVector Forward = Victim->GetActorForwardVector();
	FVector Right = Victim->GetActorRightVector();

	// HitDirection을 Victim 기준으로 투영
	FVector HitDir2D = FVector(HitDirection.X, HitDirection.Y, 0.0f).GetSafeNormal();
	FVector Forward2D = FVector(Forward.X, Forward.Y, 0.0f).GetSafeNormal();
	FVector Right2D = FVector(Right.X, Right.Y, 0.0f).GetSafeNormal();

	float ForwardDot = FVector::DotProduct(HitDir2D, Forward2D);
	float RightDot = FVector::DotProduct(HitDir2D, Right2D);

	// 45도 기준으로 방향 결정
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		// 앞/뒤 (HitDir와 Forward가 반대 방향(내적 < 0)이어야 앞에서 맞은 것)
		return (ForwardDot < 0.0f) ? EKRHitDirection::Front : EKRHitDirection::Back;
	}
	else
	{
		// 좌/우 (HitDir와 Right가 반대 방향(내적 < 0)이어야 오른쪽에서 맞은 것)
		return (RightDot < 0.0f) ? EKRHitDirection::Right : EKRHitDirection::Left;
	}
}

FString UKRHitReactionLibrary::HitDirectionToString(EKRHitDirection Direction)
{
	switch (Direction)
	{
	case EKRHitDirection::Front: return TEXT("Front");
	case EKRHitDirection::Back: return TEXT("Back");
	case EKRHitDirection::Left: return TEXT("Left");
	case EKRHitDirection::Right: return TEXT("Right");
	case EKRHitDirection::Up: return TEXT("Up");
	case EKRHitDirection::Down: return TEXT("Down");
	case EKRHitDirection::Any: return TEXT("Any");
	default: return TEXT("Unknown");
	}
}

FString UKRHitReactionLibrary::HitIntensityToString(EKRHitIntensity Intensity)
{
	switch (Intensity)
	{
	case EKRHitIntensity::Light: return TEXT("Light");
	case EKRHitIntensity::Medium: return TEXT("Medium");
	case EKRHitIntensity::Heavy: return TEXT("Heavy");
	case EKRHitIntensity::Critical: return TEXT("Critical");
	default: return TEXT("Unknown");
	}
}
