#include "KRAerialComboEffectLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UNiagaraComponent* UKRAerialComboEffectLibrary::SpawnLaunchWindEffect(
	UObject* WorldContext,
	UNiagaraSystem* NiagaraSystem,
	FVector Location,
	float Height,
	FLinearColor Color)
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

	UNiagaraComponent* WindComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		NiagaraSystem,
		Location,
		FRotator(-90.0f, 0.0f, 0.0f),  // 위쪽 방향
		FVector::OneVector,
		true,
		true,
		ENCPoolMethod::None
	);

	if (WindComp)
	{
		WindComp->SetFloatParameter(FName("User.Height"), Height);
		WindComp->SetColorParameter(FName("User.Color"), Color);
	}

	return WindComp;
}

UNiagaraComponent* UKRAerialComboEffectLibrary::StartAfterImageEffect(
	ACharacter* Character,
	UNiagaraSystem* NiagaraSystem,
	const FKRAfterImageSettings& Settings)
{
	if (!Character || !NiagaraSystem)
	{
		return nullptr;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		return nullptr;
	}

	UNiagaraComponent* AfterImageComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		NiagaraSystem,
		Mesh,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		false,  // 자동 파괴 안함
		true,
		ENCPoolMethod::None
	);

	if (AfterImageComp)
	{
		AfterImageComp->SetFloatParameter(FName("User.SpawnInterval"), Settings.SpawnInterval);
		AfterImageComp->SetFloatParameter(FName("User.FadeDuration"), Settings.FadeDuration);
		AfterImageComp->SetColorParameter(FName("User.Color"), Settings.Color);
		AfterImageComp->SetIntParameter(FName("User.MaxCount"), Settings.MaxAfterImages);
	}

	return AfterImageComp;
}

void UKRAerialComboEffectLibrary::StopAfterImageEffect(UNiagaraComponent* AfterImageComponent)
{
	if (AfterImageComponent)
	{
		AfterImageComponent->Deactivate();
	}
}

UNiagaraComponent* UKRAerialComboEffectLibrary::StartSlamTrailEffect(
	ACharacter* Character,
	UNiagaraSystem* NiagaraSystem,
	const FKRSlamTrailSettings& Settings,
	FName SocketName)
{
	if (!Character || !NiagaraSystem)
	{
		return nullptr;
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh)
	{
		return nullptr;
	}

	UNiagaraComponent* TrailComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		NiagaraSystem,
		Mesh,
		SocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		false,
		true,
		ENCPoolMethod::None
	);

	if (TrailComp)
	{
		TrailComp->SetFloatParameter(FName("User.Width"), Settings.Width);
		TrailComp->SetFloatParameter(FName("User.Length"), Settings.Length);
		TrailComp->SetColorParameter(FName("User.Color"), Settings.Color);
		TrailComp->SetFloatParameter(FName("User.GlowIntensity"), Settings.GlowIntensity);
	}

	return TrailComp;
}

void UKRAerialComboEffectLibrary::StopSlamTrailEffect(UNiagaraComponent* TrailComponent)
{
	if (TrailComponent)
	{
		TrailComponent->Deactivate();
	}
}

UNiagaraComponent* UKRAerialComboEffectLibrary::SpawnImpactWaveEffect(
	UObject* WorldContext,
	UNiagaraSystem* NiagaraSystem,
	FVector Location,
	const FKRImpactWaveSettings& Settings)
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

	UNiagaraComponent* ImpactComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		NiagaraSystem,
		Location,
		FRotator::ZeroRotator,
		FVector::OneVector,
		true,
		true,
		ENCPoolMethod::None
	);

	if (ImpactComp)
	{
		ImpactComp->SetFloatParameter(FName("User.Radius"), Settings.Radius);
		ImpactComp->SetFloatParameter(FName("User.ExpansionSpeed"), Settings.ExpansionSpeed);
		ImpactComp->SetFloatParameter(FName("User.Height"), Settings.Height);
		ImpactComp->SetColorParameter(FName("User.Color"), Settings.Color);
	}

	return ImpactComp;
}

UNiagaraComponent* UKRAerialComboEffectLibrary::SpawnGroundCrackEffect(
	UObject* WorldContext,
	UNiagaraSystem* NiagaraSystem,
	FVector Location,
	const FKRImpactWaveSettings& Settings)
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

	// 균열 끝점 계산
	TArray<FVector> CrackEndPoints = CalculateCrackEndPoints(Location, Settings.CrackCount, Settings.CrackLength);

	UNiagaraComponent* CrackComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		World,
		NiagaraSystem,
		Location,
		FRotator::ZeroRotator,
		FVector::OneVector,
		true,
		true,
		ENCPoolMethod::None
	);

	if (CrackComp)
	{
		CrackComp->SetColorParameter(FName("User.Color"), Settings.Color);
		CrackComp->SetIntParameter(FName("User.CrackCount"), Settings.CrackCount);
		CrackComp->SetFloatParameter(FName("User.CrackLength"), Settings.CrackLength);

		// 균열 끝점 배열 전달 (Niagara에서 Position Array로 사용)
		// 실제 구현에서는 NiagaraDataInterfaceArrayFunctionLibrary 사용
	}

	return CrackComp;
}

FVector UKRAerialComboEffectLibrary::GetGroundLocation(UObject* WorldContext, FVector StartLocation, float TraceDistance)
{
	if (!WorldContext)
	{
		return StartLocation;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return StartLocation;
	}

	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, TraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;

	if (World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_WorldStatic, QueryParams))
	{
		return HitResult.ImpactPoint;
	}

	return EndLocation;
}

TArray<FVector> UKRAerialComboEffectLibrary::CalculateCrackEndPoints(FVector Center, int32 CrackCount, float CrackLength)
{
	TArray<FVector> EndPoints;

	if (CrackCount <= 0)
	{
		return EndPoints;
	}

	float AngleStep = 360.0f / CrackCount;

	for (int32 i = 0; i < CrackCount; ++i)
	{
		float AngleRad = FMath::DegreesToRadians(AngleStep * i);
		
		float RandomOffset = FMath::RandRange(-15.0f, 15.0f);
		AngleRad += FMath::DegreesToRadians(RandomOffset);

		float RandomLength = CrackLength * FMath::RandRange(0.7f, 1.0f);

		FVector Direction = FVector(FMath::Cos(AngleRad), FMath::Sin(AngleRad), 0.0f);
		FVector EndPoint = Center + Direction * RandomLength;

		EndPoints.Add(EndPoint);
	}

	return EndPoints;
}
