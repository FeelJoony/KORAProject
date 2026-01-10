#include "GAS/GameplayCues/KRGameplayCueNotify_CoreDriveSurge.h"
#include "Data/DataAssets/KRStarDashData.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRStarEffectSpawner.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "TimerManager.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KRGameplayCueTag.h"
#include "SubSystem/KREffectSubsystem.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

AKRGameplayCueNotify_CoreDriveSurge::AKRGameplayCueNotify_CoreDriveSurge()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AKRGameplayCueNotify_CoreDriveSurge::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - STARTED"));

	Super::OnActive_Implementation(MyTarget, Parameters);

	if (!IsValid(MyTarget))
	{
		UE_LOG(LogTemp, Error, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - Target is Invalid!"));
		return false;
	}

	if (!StarDashData)
	{
		UE_LOG(LogTemp, Error, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - StarDashData is NULL! Check Blueprint assignments."));
		return false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - StarDashData found: %s"), *StarDashData->GetName());
	}

	OwnerActor = MyTarget;

	UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - Calling SpawnSplineLightning"));
	SpawnSplineLightning(MyTarget, Parameters);
	
	UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - Calling SpawnSlashLineEffect"));
	SpawnSlashLineEffect(MyTarget, Parameters);
	
	UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - Calling SetCharacterOpacity"));
	SetCharacterOpacity(MyTarget, StarDashData->Visual.DashOpacity);
	
	UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - Calling StartAfterImageEffect"));
	StartAfterImageEffect(MyTarget);

	UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::OnActive - COMPLETED successfully"));
	return true;
}

bool AKRGameplayCueNotify_CoreDriveSurge::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	return Super::WhileActive_Implementation(MyTarget, Parameters);
}

bool AKRGameplayCueNotify_CoreDriveSurge::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	Super::OnRemove_Implementation(MyTarget, Parameters);
	
	if (SpawnedSplineLightningActor)
	{
		SpawnedSplineLightningActor->Destroy();
	}

	if (SpawnedSlashEffectComponent)
	{
		SpawnedSlashEffectComponent->Deactivate();
		SpawnedSlashEffectComponent = nullptr;
	}
	
	ResetCharacterOpacity(OwnerActor.Get());
	StopAfterImageEffect();

	return true;
}

void AKRGameplayCueNotify_CoreDriveSurge::SpawnSplineLightning(AActor* Target, const FGameplayCueParameters& Parameters)
{
	if (!StarDashData || !StarDashData->Effects.bEnableLightning || !StarDashData->Effects.SplineLightningClass)
	{
		return;
	}

	// Check if parameters are valid (triggered by Event) or empty (triggered by Tag)
	const bool bUseParams = !Parameters.Location.IsZero() && Parameters.RawMagnitude > 0.0f;

	const FVector StartLocation = bUseParams ? FVector(Parameters.Location) : Target->GetActorLocation();
	const FVector Direction = bUseParams ? FVector(Parameters.Normal) : Target->GetActorForwardVector();
	const float Distance = bUseParams ? Parameters.RawMagnitude : StarDashData->Pattern.OuterRadius;
	
	const FVector EndLocation = StartLocation + Direction * Distance;

	SpawnedSplineLightningActor = UKRStarEffectSpawner::SpawnSplineLightningActor(
		Target,
		StarDashData->Effects.SplineLightningClass,
		StartLocation,
		EndLocation,
		StarDashData->Timing.LineDrawDuration,
		-1
	);
}

void AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect(AActor* Target, const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - STARTED"));

	if (!StarDashData)
	{
		UE_LOG(LogTemp, Error, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - StarDashData is NULL!"));
		return;
	}

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - GetWorld() returned NULL!"));
		return;
	}

	const FGameplayTag& EffectTag = StarDashData->Effects.SlashLineEffectTag;
	if (!EffectTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - SlashLineEffectTag is Invalid! Check DataAsset."));
		return;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - Effect Tag: %s"), *EffectTag.ToString());
	}

	UKREffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UKREffectSubsystem>();
	if (!EffectSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - EffectSubsystem Not Found!"));
		return;
	}

	// Prepare spawn parameters
	const bool bUseParams = !Parameters.Location.IsZero() && Parameters.RawMagnitude > 0.0f;
	const FVector StartLocation = bUseParams ? FVector(Parameters.Location) : Target->GetActorLocation();
	const FVector Direction = bUseParams ? FVector(Parameters.Normal) : Target->GetActorForwardVector();
	const float Distance = bUseParams ? Parameters.RawMagnitude : StarDashData->Pattern.OuterRadius;
	const FVector EndLocation = StartLocation + Direction * Distance;

	UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - Spawning from %s to %s"), *StartLocation.ToString(), *EndLocation.ToString());

	// Construct points array for Niagara
	TArray<FVector> Points;
	Points.Add(StartLocation);
	Points.Add(EndLocation);

	// Spawn effect via subsystem
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(StartLocation); 
	
	SpawnedSlashEffectComponent = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, Target));

	if (SpawnedSlashEffectComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - Niagara Component Spawned Successfully!"));

		// Set Parameters based on Data Asset
		SpawnedSlashEffectComponent->SetFloatParameter(FName("User.DrawDuration"), StarDashData->Timing.LineDrawDuration);
		SpawnedSlashEffectComponent->SetColorParameter(FName("User.LineColor"), StarDashData->Effects.LineColor);
		SpawnedSlashEffectComponent->SetFloatParameter(FName("User.LineWidth"), StarDashData->Effects.LineWidth);
		SpawnedSlashEffectComponent->SetFloatParameter(FName("User.GlowIntensity"), StarDashData->Effects.GlowIntensity);
		
		// Set Points
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
			SpawnedSlashEffectComponent,
			FName("User.Points"),
			Points
		);
		UE_LOG(LogTemp, Warning, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - Parameters Set. DrawDuration: %f"), StarDashData->Timing.LineDrawDuration);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AKRGameplayCueNotify_CoreDriveSurge::SpawnSlashLineEffect - Failed to spawn Niagara Component! Check KREffectSubsystem or Tag registration."));
	}
}

void AKRGameplayCueNotify_CoreDriveSurge::SetCharacterOpacity(AActor* Target, float Opacity)
{
	if (!IsValid(Target) || !StarDashData) return;

	ACharacter* Character = Cast<ACharacter>(Target);
	if (!Character) return;

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	if (!Mesh) return;

	CachedMesh = Mesh;
	CachedOriginalMaterials.Empty();
	
	const int32 MaterialCount = Mesh->GetNumMaterials();
	const FName OpacityParamName = StarDashData->Visual.OpacityParameterName;
	
	for (int32 i = 0; i < MaterialCount; ++i)
	{
		UMaterialInterface* OriginalMaterial = Mesh->GetMaterial(i);
		CachedOriginalMaterials.Add(OriginalMaterial);

		if (OriginalMaterial)
		{
			UMaterialInstanceDynamic* DynamicMaterial = Mesh->CreateDynamicMaterialInstance(i, OriginalMaterial);
			if (DynamicMaterial)
			{
				DynamicMaterial->SetScalarParameterValue(OpacityParamName, Opacity);
			}
		}
	}
}

void AKRGameplayCueNotify_CoreDriveSurge::ResetCharacterOpacity(AActor* Target)
{
	if (USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(CachedMesh.Get()))
	{
		const int32 MaterialCount = Mesh->GetNumMaterials();
		// Ensure we don't go out of bounds if material count changed (unlikely but safe)
		const int32 Limit = FMath::Min(MaterialCount, CachedOriginalMaterials.Num());

		for (int32 i = 0; i < Limit; ++i)
		{
			if (CachedOriginalMaterials[i] && Mesh->GetMaterial(i) != CachedOriginalMaterials[i])
			{
				Mesh->SetMaterial(i, CachedOriginalMaterials[i]);
			}
		}
	}
	
	CachedMesh.Reset();
	CachedOriginalMaterials.Empty();
}

void AKRGameplayCueNotify_CoreDriveSurge::StartAfterImageEffect(AActor* Target)
{
	if (!StarDashData || !IsValid(Target) || !GetWorld()) return;
	
	const FGameplayTag& EffectTag = StarDashData->Effects.AfterImageEffectTag;
	if (!EffectTag.IsValid()) return;

	ACharacter* Character = Cast<ACharacter>(Target);
	if (!Character) return;

	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	if (!MeshComp) return;

	UKREffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem)
	{
		SpawnedAfterImageComponent = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectAttached(
			EffectTag,
			MeshComp,
			NAME_None
		));

		if (SpawnedAfterImageComponent)
		{
			// Skeletal Mesh Parameter
			SpawnedAfterImageComponent->SetVariableObject(FName("User.SourceSkeletalMesh"), MeshComp);

			// Color
			FLinearColor OpaqueColor = StarDashData->Effects.AfterImageColor;
			OpaqueColor.A = 1.0f;
			SpawnedAfterImageComponent->SetColorParameter(FName("Color"), OpaqueColor);
			SpawnedAfterImageComponent->SetColorParameter(FName("User.Color"), OpaqueColor);
			SpawnedAfterImageComponent->SetColorParameter(FName("AfterImageColor"), OpaqueColor);

			// Lifetime / Duration
			SpawnedAfterImageComponent->SetFloatParameter(FName("Lifetime"), 0.5f);
			SpawnedAfterImageComponent->SetFloatParameter(FName("User.Lifetime"), 0.5f);
			SpawnedAfterImageComponent->SetFloatParameter(FName("Duration"), 0.5f);
			SpawnedAfterImageComponent->SetFloatParameter(FName("FadeTime"), 0.3f);

			// Spawn Rate
			SpawnedAfterImageComponent->SetFloatParameter(FName("SpawnRate"), 30.0f);
			SpawnedAfterImageComponent->SetFloatParameter(FName("User.SpawnRate"), 30.0f);
			SpawnedAfterImageComponent->SetIntParameter(FName("SpawnCount"), 5);

			// Opacity
			SpawnedAfterImageComponent->SetFloatParameter(FName("Opacity"), 0.8f);
			SpawnedAfterImageComponent->SetFloatParameter(FName("User.Opacity"), 0.8f);
			SpawnedAfterImageComponent->SetFloatParameter(FName("Alpha"), 0.8f);
		}
	}
}

void AKRGameplayCueNotify_CoreDriveSurge::StopAfterImageEffect()
{
	if (SpawnedAfterImageComponent)
	{
		SpawnedAfterImageComponent->Deactivate();
		SpawnedAfterImageComponent = nullptr;
	}
}
