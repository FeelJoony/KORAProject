#include "KRStarEffectSpawner.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "TimerManager.h"
#include "Components/SplineComponent.h"

UNiagaraComponent* UKRStarEffectSpawner::SpawnStarLineEffect(
	UObject* WorldContext,
	const FKRStarPatternData& PatternData,
	const FKRStarEffectSettings& Settings)
{
	if (!WorldContext || !Settings.NiagaraSystem)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		Settings.NiagaraSystem,
		PatternData.CenterLocation,
		FRotator::ZeroRotator,
		FVector::OneVector,
		true,
		true,
		ENCPoolMethod::None
	);

	if (NiagaraComp)
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
			NiagaraComp,
			FName("User.Points"),
			PatternData.DrawOrderPoints
		);

		NiagaraComp->SetColorParameter(FName("User.LineColor"), Settings.LineColor);
		NiagaraComp->SetFloatParameter(FName("User.LineWidth"), Settings.LineWidth);
		NiagaraComp->SetFloatParameter(FName("User.DrawDuration"), Settings.DrawDuration);
		NiagaraComp->SetFloatParameter(FName("User.GlowIntensity"), Settings.GlowIntensity);
		NiagaraComp->SetFloatParameter(FName("User.DrawAlpha"), 0.0f);
	}

	return NiagaraComp;
}

UNiagaraComponent* UKRStarEffectSpawner::SpawnStarEffectAroundActor(
	UObject* WorldContext,
	AActor* TargetActor,
	float Radius,
	const FKRStarEffectSettings& Settings,
	float HeightOffset)
{
	if (!TargetActor)
	{
		return nullptr;
	}

	FKRStarPatternData PatternData = UKRStarPatternLibrary::GenerateStarPatternAroundActor(
		TargetActor,
		Radius,
		HeightOffset,
		true
	);

	return SpawnStarLineEffect(WorldContext, PatternData, Settings);
}

UNiagaraComponent* UKRStarEffectSpawner::SpawnSingleLineEffect(
	UObject* WorldContext,
	UNiagaraSystem* NiagaraSystem,
	FVector StartPoint,
	FVector EndPoint,
	FLinearColor Color,
	float Width,
	float DrawDuration,
	float GlowIntensity)
{
	if (!WorldContext || !NiagaraSystem)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	FVector MidPoint = (StartPoint + EndPoint) * 0.5f;

	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		NiagaraSystem,
		MidPoint,
		FRotator::ZeroRotator,
		FVector::OneVector,
		true,
		true,
		ENCPoolMethod::None
	);

	if (NiagaraComp)
	{
		TArray<FVector> Points;
		Points.Add(StartPoint);
		Points.Add(EndPoint);

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
			NiagaraComp,
			FName("User.Points"),
			Points
		);

		NiagaraComp->SetColorParameter(FName("User.LineColor"), Color);
		NiagaraComp->SetFloatParameter(FName("User.LineWidth"), Width);
		NiagaraComp->SetFloatParameter(FName("User.DrawDuration"), DrawDuration);
		NiagaraComp->SetFloatParameter(FName("User.GlowIntensity"), GlowIntensity);
		NiagaraComp->SetFloatParameter(FName("User.DrawAlpha"), 1.0f);
	}

	return NiagaraComp;
}

void UKRStarEffectSpawner::UpdateStarEffectReveal(UNiagaraComponent* NiagaraComponent, float Alpha)
{
	if (!NiagaraComponent)
	{
		return;
	}

	float ClampedAlpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	NiagaraComponent->SetFloatParameter(FName("User.DrawAlpha"), ClampedAlpha);
}

void UKRStarEffectSpawner::StartStarDrawAnimation(
	UObject* WorldContext,
	UNiagaraComponent* NiagaraComponent,
	float Duration)
{
	if (!WorldContext || !NiagaraComponent || Duration <= 0.0f)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	TWeakObjectPtr<UNiagaraComponent> WeakNiagara = NiagaraComponent;
	float StartTime = World->GetTimeSeconds();
	float EndTime = StartTime + Duration;

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(
		TimerHandle,
		[WeakNiagara, World, StartTime, EndTime]()
		{
			if (!WeakNiagara.IsValid())
			{
				return;
			}

			float CurrentTime = World->GetTimeSeconds();
			float Progress = FMath::Clamp((CurrentTime - StartTime) / (EndTime - StartTime), 0.0f, 1.0f);

			UpdateStarEffectReveal(WeakNiagara.Get(), Progress);
		},
		0.016f,
		true
	);

	FTimerHandle StopHandle;
	World->GetTimerManager().SetTimer(
		StopHandle,
		[World, TimerHandle]() mutable
		{
			World->GetTimerManager().ClearTimer(TimerHandle);
		},
		Duration + 0.1f,
		false
	);
}

static FVector PickPerpAxis(const FVector& Dir)
{
	FVector Right = FVector::CrossProduct(Dir, FVector::UpVector);
	if (Right.SizeSquared() < 1e-4f)
	{
		Right = FVector::CrossProduct(Dir, FVector::RightVector);
	}
	return Right.GetSafeNormal();
}

static void BuildLightningSplinePoints(
	const FVector& Start,
	const FVector& End,
	int32 NumSegments,
	float ArcAmplitude,
	float JitterAmplitude,
	FRandomStream& RandomStream,
	TArray<FVector>& OutWorldPoints)
{
	OutWorldPoints.Reset();
	OutWorldPoints.Reserve(NumSegments + 1);

	const FVector AB = End - Start;
	const float Len = AB.Size();
	const FVector Dir = (Len > KINDA_SMALL_NUMBER) ? (AB / Len) : FVector::ForwardVector;

	const FVector Right = PickPerpAxis(Dir);
	const FVector Up = FVector::CrossProduct(Right, Dir).GetSafeNormal();

	for (int32 i = 0; i <= NumSegments; ++i)
	{
		const float T = static_cast<float>(i) / static_cast<float>(NumSegments);

		FVector P = FMath::Lerp(Start, End, T);

		if (i > 0 && i < NumSegments)
		{
			const float Arc = FMath::Sin(T * PI) * ArcAmplitude;
			const float JitR = RandomStream.FRandRange(-JitterAmplitude, JitterAmplitude);
			const float JitU = RandomStream.FRandRange(-JitterAmplitude, JitterAmplitude);

			P += Right * (Arc + JitR);
			P += Up * JitU;
		}

		OutWorldPoints.Add(P);
	}
}

AActor* UKRStarEffectSpawner::SpawnSplineLightningActor(
	UObject* WorldContext,
	TSubclassOf<AActor> SplineVFXClass,
	FVector StartPoint,
	FVector EndPoint,
	float LifeSpan,
	int32 Seed)
{
	if (!WorldContext || !SplineVFXClass)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedActor = World->SpawnActor<AActor>(SplineVFXClass, StartPoint, FRotator::ZeroRotator, SpawnParams);
	if (!SpawnedActor)
	{
		return nullptr;
	}

	USplineComponent* SplineComp = SpawnedActor->FindComponentByClass<USplineComponent>();
	if (!SplineComp)
	{
		return SpawnedActor;
	}

	const float Len = FVector::Dist(StartPoint, EndPoint);
	const int32 Segments = FMath::Clamp(FMath::RoundToInt(Len / 80.0f), 6, 18);
	const float ArcAmp = FMath::Clamp(Len * 0.08f, 15.0f, 80.0f);
	const float Jitter = FMath::Clamp(Len * 0.03f, 6.0f, 35.0f);

	FRandomStream RandomStream;
	if (Seed >= 0)
	{
		RandomStream.Initialize(Seed);
	}
	else
	{
		RandomStream.Initialize(FMath::Rand());
	}

	TArray<FVector> WorldPoints;
	BuildLightningSplinePoints(StartPoint, EndPoint, Segments, ArcAmp, Jitter, RandomStream, WorldPoints);

	SplineComp->ClearSplinePoints(false);

	const FTransform& ActorTM = SpawnedActor->GetActorTransform();
	for (int32 i = 0; i < WorldPoints.Num(); ++i)
	{
		const FVector LocalP = ActorTM.InverseTransformPosition(WorldPoints[i]);
		SplineComp->AddSplinePoint(LocalP, ESplineCoordinateSpace::Local, false);
		SplineComp->SetSplinePointType(i, ESplinePointType::CurveClamped, false);
	}

	SplineComp->UpdateSpline();

	if (LifeSpan > 0.0f)
	{
		SpawnedActor->SetLifeSpan(LifeSpan);
	}

	return SpawnedActor;
}

void UKRStarEffectSpawner::GenerateCurvedLightningPoints(
	FVector StartPoint,
	FVector EndPoint,
	int32 Seed,
	TArray<FVector>& OutPoints)
{
	const float Len = FVector::Dist(StartPoint, EndPoint);
	const int32 Segments = FMath::Clamp(FMath::RoundToInt(Len / 80.0f), 6, 18);
	const float ArcAmp = FMath::Clamp(Len * 0.08f, 15.0f, 80.0f);
	const float Jitter = FMath::Clamp(Len * 0.03f, 6.0f, 35.0f);

	FRandomStream RandomStream;
	if (Seed >= 0)
	{
		RandomStream.Initialize(Seed);
	}
	else
	{
		RandomStream.Initialize(FMath::Rand());
	}

	BuildLightningSplinePoints(StartPoint, EndPoint, Segments, ArcAmp, Jitter, RandomStream, OutPoints);
}
