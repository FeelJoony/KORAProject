#include "KRAnimNotifyState_TimedCombatEffect.h"
#include "SubSystem/KREffectSubsystem.h"
#include "Data/DataAssets/KREffectDefinition.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"

UKRAnimNotifyState_TimedCombatEffect::UKRAnimNotifyState_TimedCombatEffect()
{
	bIsNativeBranchingPoint = false;
}

void UKRAnimNotifyState_TimedCombatEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (SpawnedEffect.IsValid())
	{
		SpawnedEffect->Deactivate();
		SpawnedEffect.Reset();
	}

	if (!MeshComp || !MeshComp->GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("  FAILED: MeshComp or World is null"));
		return;
	}

	if (!EffectTag.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("  FAILED: EffectTag is not valid"));
		return;
	}

	UKREffectSubsystem* EffectSubsystem = MeshComp->GetWorld()->GetSubsystem<UKREffectSubsystem>();
	if (!EffectSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("  FAILED: EffectSubsystem is null"));
		return;
	}

	UKREffectDefinition* EffectDef = EffectSubsystem->GetEffectDefinition(EffectTag);
	if (!EffectDef)
	{
		UE_LOG(LogTemp, Error, TEXT("  FAILED: EffectDefinition not found for tag %s"), *EffectTag.ToString());
		return;
	}
	AActor* Owner = MeshComp->GetOwner();
	const FKREffectSpawnSettings& SpawnSettings = EffectDef->SpawnSettings;

	FName SocketToUse = NAME_None;
	if (SpawnSettings.bAttachToParent)
	{
		SocketToUse = SpawnSettings.AttachSocketName;
	}

	if (SocketToUse != NAME_None)
	{
		FVector SocketLocation = MeshComp->GetSocketLocation(SocketToUse);
		bool bSocketExists = MeshComp->DoesSocketExist(SocketToUse);
	}

	if (SocketToUse != NAME_None)
	{
		SpawnedEffect = EffectSubsystem->SpawnEffectAttached(
			EffectTag,
			MeshComp,
			SocketToUse,
			SpawnSettings.LocationOffset,
			SpawnSettings.RotationOffset
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  Calling SpawnEffectByTag (no socket)..."));
		FTransform SpawnTransform = Owner ? Owner->GetActorTransform() : MeshComp->GetComponentTransform();
		SpawnTransform.SetLocation(SpawnTransform.GetLocation() + SpawnSettings.LocationOffset);
		SpawnedEffect = EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, Owner);
	}
}

void UKRAnimNotifyState_TimedCombatEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (SpawnedEffect.IsValid())
	{
		SpawnedEffect->Deactivate();
		SpawnedEffect.Reset();
	}
}

FString UKRAnimNotifyState_TimedCombatEffect::GetNotifyName_Implementation() const
{
	if (EffectTag.IsValid())
	{
		return FString::Printf(TEXT("Timed Effect: %s"), *EffectTag.ToString());
	}
	return TEXT("Timed Combat Effect");
}
