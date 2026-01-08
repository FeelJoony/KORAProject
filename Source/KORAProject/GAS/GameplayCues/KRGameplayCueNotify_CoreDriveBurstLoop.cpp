#include "GAS/GameplayCues/KRGameplayCueNotify_CoreDriveBurstLoop.h"
#include "Data/DataAssets/KRStarDashData.h"
#include "SubSystem/KREffectSubsystem.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GAS/Abilities/HeroAbilities/KRGA_CoreDriveBurst_Sword.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRStarEffectSpawner.h"

AKRGameplayCueNotify_CoreDriveBurstLoop::AKRGameplayCueNotify_CoreDriveBurstLoop()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AKRGameplayCueNotify_CoreDriveBurstLoop::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	UE_LOG(LogTemp, Warning, TEXT("BurstLoop GCN: OnActive Called on %s"), *GetNameSafe(MyTarget));

	Super::OnActive_Implementation(MyTarget, Parameters);

	if (!IsValid(MyTarget))
	{
		UE_LOG(LogTemp, Error, TEXT("BurstLoop GCN: Target is Invalid!"));
		return false;
	}
	
	if (!StarDashData)
	{
		UE_LOG(LogTemp, Error, TEXT("BurstLoop GCN: StarDashData is NULL! Check Blueprint."));
		return false;
	}

	OwnerActor = MyTarget;

	SetCharacterOpacity(MyTarget, StarDashData->Visual.DashOpacity);
	StartAfterImage(MyTarget);

	// GA 찾아서 좌표 가져오기
	TargetPoints.Empty();
	
	// GA에서 AddGameplayCue 호출 시 SourceObject에 'this'(Ability)를 넣어 보냈으므로 바로 캐스팅 가능
	if (const UKRGA_CoreDriveBurst_Sword* GA = Cast<UKRGA_CoreDriveBurst_Sword>(Parameters.SourceObject.Get()))
	{
		TargetPoints = GA->GetCachedWorldPoints();
		UE_LOG(LogTemp, Warning, TEXT("BurstLoop GCN: Found GA via SourceObject! Points Num: %d"), TargetPoints.Num());
	}
	else
	{
		// Fallback: 기존 ASC 검색 방식 (혹시 EffectCauser가 설정 안 된 경우 대비)
		// MyTarget(Avatar)을 통해 ASC 가져오기
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MyTarget))
		{
			TArray<FGameplayAbilitySpec> Specs = ASC->GetActivatableAbilities();
			for (const FGameplayAbilitySpec& Spec : Specs)
			{
				if (Spec.Ability && Spec.Ability->IsA<UKRGA_CoreDriveBurst_Sword>())
				{
					if (UKRGA_CoreDriveBurst_Sword* FoundGA = Cast<UKRGA_CoreDriveBurst_Sword>(Spec.GetPrimaryInstance()))
					{
						TargetPoints = FoundGA->GetCachedWorldPoints();
						UE_LOG(LogTemp, Warning, TEXT("BurstLoop GCN: Found GA via ASC Search! Points Num: %d"), TargetPoints.Num());
						break;
					}
				}
			}
		}
	}

	if (TargetPoints.Num() > 0)
	{
		// 라인 이펙트는 전체 경로를 한 번에 그립니다
		SpawnLineEffect();
		
		// 번개는 순차적으로 그립니다
		StartEffectSequence();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BurstLoop GCN: TargetPoints is Empty! Effect sequence will not run."));
	}

	return true;
}

bool AKRGameplayCueNotify_CoreDriveBurstLoop::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	Super::OnRemove_Implementation(MyTarget, Parameters);

	ResetCharacterOpacity(MyTarget);
	StopAfterImage();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SequenceTimerHandle);

		// Line Effect 일괄 소멸
		if (StarDashData && StarDashData->Effects.SlashLineEffectTag.IsValid())
		{
			if (UKREffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UKREffectSubsystem>())
			{
				EffectSubsystem->StopEffectByTag(StarDashData->Effects.SlashLineEffectTag);
			}
		}
	}

	return true;
}

void AKRGameplayCueNotify_CoreDriveBurstLoop::SetCharacterOpacity(AActor* Target, float Opacity)
{
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

void AKRGameplayCueNotify_CoreDriveBurstLoop::ResetCharacterOpacity(AActor* Target)
{
	if (USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(CachedMesh.Get()))
	{
		const int32 MaterialCount = Mesh->GetNumMaterials();
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

void AKRGameplayCueNotify_CoreDriveBurstLoop::StartAfterImage(AActor* Target)
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
			SpawnedAfterImageComponent->SetVariableObject(FName("User.SourceSkeletalMesh"), MeshComp);

			FLinearColor OpaqueColor = StarDashData->Effects.AfterImageColor;
			SpawnedAfterImageComponent->SetColorParameter(FName("Color"), OpaqueColor);
			SpawnedAfterImageComponent->SetColorParameter(FName("User.Color"), OpaqueColor);
			SpawnedAfterImageComponent->SetColorParameter(FName("AfterImageColor"), OpaqueColor);
			
			SpawnedAfterImageComponent->SetFloatParameter(FName("Lifetime"), 0.5f);
			SpawnedAfterImageComponent->SetFloatParameter(FName("User.Lifetime"), 0.5f);
		}
	}
}

void AKRGameplayCueNotify_CoreDriveBurstLoop::StopAfterImage()
{
	if (SpawnedAfterImageComponent)
	{
		SpawnedAfterImageComponent->Deactivate();
		SpawnedAfterImageComponent = nullptr;
	}
}

void AKRGameplayCueNotify_CoreDriveBurstLoop::StartEffectSequence()
{
	CurrentSequenceIndex = 0;
	
	// 첫 번째 이펙트 즉시 실행
	PerformNextEffect();

	float Interval = StarDashData->Timing.DashInterval;
	if (Interval <= 0.0f) Interval = 0.1f;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SequenceTimerHandle, this, &ThisClass::PerformNextEffect, Interval, true);
	}
}

void AKRGameplayCueNotify_CoreDriveBurstLoop::PerformNextEffect()
{
	if (TargetPoints.Num() < 2) return;

	int32 StartIdx = CurrentSequenceIndex % TargetPoints.Num();
	int32 EndIdx = (CurrentSequenceIndex + 1) % TargetPoints.Num();

	FVector Start = TargetPoints[StartIdx];
	FVector End = TargetPoints[EndIdx];

	SpawnLightningEffect(Start, End);

	CurrentSequenceIndex++;
}

void AKRGameplayCueNotify_CoreDriveBurstLoop::SpawnLineEffect()
{
	if (!StarDashData || !GetWorld() || TargetPoints.Num() < 2) return;

	const FGameplayTag& EffectTag = StarDashData->Effects.SlashLineEffectTag;
	if (!EffectTag.IsValid()) return;

	UKREffectSubsystem* EffectSubsystem = GetWorld()->GetSubsystem<UKREffectSubsystem>();
	if (!EffectSubsystem) return;

	// 시작 위치 (첫 번째 포인트)
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(TargetPoints[0]);

	UNiagaraComponent* Component = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, OwnerActor.Get()));
	if (Component)
	{
		Component->SetFloatParameter(FName("User.DrawDuration"), StarDashData->Timing.LineDrawDuration);
		Component->SetColorParameter(FName("User.LineColor"), StarDashData->Effects.LineColor);
		Component->SetFloatParameter(FName("User.LineWidth"), StarDashData->Effects.LineWidth);
		Component->SetFloatParameter(FName("User.GlowIntensity"), StarDashData->Effects.GlowIntensity);
		
		// 전체 포인트 배열 전달
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
			Component,
			FName("User.Points"),
			TargetPoints
		);
	}
}

void AKRGameplayCueNotify_CoreDriveBurstLoop::SpawnLightningEffect(const FVector& Start, const FVector& End)
{
	if (!StarDashData || !StarDashData->Effects.bEnableLightning || !StarDashData->Effects.SplineLightningClass)
	{
		return;
	}
	
	UKRStarEffectSpawner::SpawnSplineLightningActor(
		OwnerActor.Get(),
		StarDashData->Effects.SplineLightningClass,
		Start,
		End,
		StarDashData->Timing.LineDrawDuration,
		-1
	);
}
