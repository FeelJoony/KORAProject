#include "KRMeleeAttackTypes.h"
#include "DrawDebugHelpers.h"

FString UKRMeleeAttackLibrary::AttackShapeToString(EKRAttackShapeType Shape)
{
	switch (Shape)
	{
	case EKRAttackShapeType::Box: return TEXT("Box");
	case EKRAttackShapeType::Sphere: return TEXT("Sphere");
	case EKRAttackShapeType::Cone: return TEXT("Cone");
	case EKRAttackShapeType::Capsule: return TEXT("Capsule");
	default: return TEXT("Unknown");
	}
}

void UKRMeleeAttackLibrary::DrawDebugAttackShape(
	const UObject* WorldContextObject,
	const FVector& Origin,
	const FVector& Forward,
	const FKRMeleeAttackConfig& Config,
	FColor Color,
	float Duration)
{
#if ENABLE_DRAW_DEBUG
	if (!WorldContextObject) return;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return;

	const FVector NormalizedForward = Forward.GetSafeNormal();
	const FVector EndPoint = Origin + NormalizedForward * Config.AttackRange;

	switch (Config.AttackShape)
	{
	case EKRAttackShapeType::Box:
		{
			// 박스 형태 시각화
			const FQuat Rotation = FRotationMatrix::MakeFromX(NormalizedForward).ToQuat();
			const FVector Center = Origin + Config.AddStartLocation + NormalizedForward * (Config.AttackRange * 0.5f);
			DrawDebugBox(World, Center, Config.BoxExtent, Rotation, Color, false, Duration);
		}
		break;

	case EKRAttackShapeType::Sphere:
		{
			// 구체 형태 시각화 (Origin에서 전방으로)
			DrawDebugSphere(World, EndPoint, Config.SphereRadius, 12, Color, false, Duration);
			DrawDebugLine(World, Origin, EndPoint, Color, false, Duration);
		}
		break;

	case EKRAttackShapeType::Cone:
		{
			// 원뿔 형태 시각화 (여러 라인으로)
			const float HalfAngle = Config.ConeAngle * 0.5f;
			const int32 NumLines = FMath::Max(5, static_cast<int32>(Config.ConeAngle / 15.0f));
			const float AngleStep = Config.ConeAngle / (NumLines - 1);
			const float StartAngle = -HalfAngle;

			const FVector Up = FVector::UpVector;

			for (int32 i = 0; i < NumLines; ++i)
			{
				const float CurrentAngle = StartAngle + (AngleStep * i);
				const FVector Direction = NormalizedForward.RotateAngleAxis(CurrentAngle, Up);
				const FVector LineEnd = Origin + Direction * Config.AttackRange;
				DrawDebugLine(World, Origin, LineEnd, Color, false, Duration);
			}

			// 원호 시각화 (끝점 연결)
			for (int32 i = 0; i < NumLines - 1; ++i)
			{
				const float Angle1 = StartAngle + (AngleStep * i);
				const float Angle2 = StartAngle + (AngleStep * (i + 1));
				const FVector Dir1 = NormalizedForward.RotateAngleAxis(Angle1, Up);
				const FVector Dir2 = NormalizedForward.RotateAngleAxis(Angle2, Up);
				const FVector End1 = Origin + Dir1 * Config.AttackRange;
				const FVector End2 = Origin + Dir2 * Config.AttackRange;
				DrawDebugLine(World, End1, End2, Color, false, Duration);
			}
		}
		break;

	case EKRAttackShapeType::Capsule:
		{
			// 캡슐 형태 시각화
			const FQuat Rotation = FRotationMatrix::MakeFromZ(NormalizedForward).ToQuat();
			const FVector Center = Origin + NormalizedForward * (Config.AttackRange * 0.5f);
			DrawDebugCapsule(World, Center, Config.CapsuleHalfHeight, Config.CapsuleRadius, Rotation, Color, false, Duration);
		}
		break;
	}
#endif
}
