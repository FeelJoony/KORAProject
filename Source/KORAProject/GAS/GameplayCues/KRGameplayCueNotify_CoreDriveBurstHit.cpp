#include "GAS/GameplayCues/KRGameplayCueNotify_CoreDriveBurstHit.h"
#include "Data/DataAssets/KRStarDashData.h"
#include "SubSystem/KREffectSubsystem.h"
#include "GAS/Abilities/HeroAbilities/Libraries/KRStarEffectSpawner.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

UKRGameplayCueNotify_CoreDriveBurstHit::UKRGameplayCueNotify_CoreDriveBurstHit()
{
}

void UKRGameplayCueNotify_CoreDriveBurstHit::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters)
{
	// Static GCN handles OnExecute event usually
	if (EventType != EGameplayCueEvent::Executed)
	{
		return;
	}

	Super::HandleGameplayCue(MyTarget, EventType, Parameters);

	// 1. 파라미터로 전달된 DataAsset 가져오기
	const UKRStarDashData* StarDashData = Cast<UKRStarDashData>(Parameters.SourceObject);
	
	if (!StarDashData)
	{
		return;
	}

	FVector Start = Parameters.Location;
	FVector End = FVector::ZeroVector;
	if (Parameters.EffectContext.IsValid())
	{
		End = Parameters.EffectContext.GetOrigin();
	}
	else
	{
		return;
	}

	bool bIsFinisher = FMath::IsNearlyEqual(Parameters.RawMagnitude, 1.0f);

	// Lightning은 항상 발생
	SpawnLightningEffect(MyTarget, Parameters, StarDashData);

	// Line은 Finisher가 아닐 때만 발생
	if (!bIsFinisher)
	{
		SpawnLineEffect(MyTarget, Parameters, StarDashData);
	}
}

void UKRGameplayCueNotify_CoreDriveBurstHit::SpawnLineEffect(AActor* Target, const FGameplayCueParameters& Parameters, const UKRStarDashData* Data) const
{
	if (!Data || !Target || !Target->GetWorld()) return;

	const FGameplayTag& EffectTag = Data->Effects.SlashLineEffectTag;
	if (!EffectTag.IsValid()) return;

	UKREffectSubsystem* EffectSubsystem = Target->GetWorld()->GetSubsystem<UKREffectSubsystem>();
	if (!EffectSubsystem) return;

	FVector Start = Parameters.Location;
	FVector End = Parameters.EffectContext.GetOrigin();

	TArray<FVector> Points;
	Points.Add(Start);
	Points.Add(End);

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(Start);

	UNiagaraComponent* Component = Cast<UNiagaraComponent>(EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, Target));
	if (Component)
	{
		Component->SetFloatParameter(FName("User.DrawDuration"), Data->Timing.LineDrawDuration);
		Component->SetColorParameter(FName("User.LineColor"), Data->Effects.LineColor);
		Component->SetFloatParameter(FName("User.LineWidth"), Data->Effects.LineWidth);
		Component->SetFloatParameter(FName("User.GlowIntensity"), Data->Effects.GlowIntensity);
		
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
			Component,
			FName("User.Points"),
			Points
		);
	}
}

void UKRGameplayCueNotify_CoreDriveBurstHit::SpawnLightningEffect(AActor* Target, const FGameplayCueParameters& Parameters, const UKRStarDashData* Data) const
{
	if (!Data || !Data->Effects.bEnableLightning || !Data->Effects.SplineLightningClass)
	{
		return;
	}
	
	FVector Start = Parameters.Location;
	FVector End = Parameters.EffectContext.GetOrigin();
	
	UKRStarEffectSpawner::SpawnSplineLightningActor(
		Target,
		Data->Effects.SplineLightningClass,
		Start,
		End,
		Data->Timing.LineDrawDuration,
		-1 // Random Seed
	);
}
