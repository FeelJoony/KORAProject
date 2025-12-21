#include "KRAnimNotify_Footstep.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "SubSystem/KREffectSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

void UKRAnimNotify_Footstep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	UWorld* World = MeshComp->GetWorld();
	if (!Owner || !World)
	{
		return;
	}

	UPhysicalMaterial* PhysMat = DetectGroundSurface(MeshComp, FootSocketName);
	FGameplayTag SoundTag = GetSoundTagForSurface(PhysMat, FootstepType);
	FGameplayTag EffectTag = GetEffectTagForSurface(PhysMat, FootstepType);
	FVector FootLocation = MeshComp->GetSocketLocation(FootSocketName);

	UKRSoundSubsystem* SoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>();
	if (SoundSubsystem && SoundTag.IsValid())
	{
		UAudioComponent* AudioComp = SoundSubsystem->PlaySoundByTag(SoundTag, FootLocation, Owner);
		if (AudioComp)
		{
			AudioComp->SetVolumeMultiplier(AudioComp->VolumeMultiplier * VolumeMultiplier);
		}
	}

	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (EffectSubsystem && EffectTag.IsValid())
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(FootLocation);
		EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, Owner);
	}
}

UPhysicalMaterial* UKRAnimNotify_Footstep::DetectGroundSurface(USkeletalMeshComponent* MeshComp, const FName& SocketName) const
{
	if (!MeshComp)
	{
		return nullptr;
	}

	UWorld* World = MeshComp->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FVector StartLocation = MeshComp->GetSocketLocation(SocketName);
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 50.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(MeshComp->GetOwner());

	bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);

	if (bHit && HitResult.PhysMaterial.IsValid())
	{
		return HitResult.PhysMaterial.Get();
	}

	return nullptr;
}

FGameplayTag UKRAnimNotify_Footstep::GetSoundTagForSurface(UPhysicalMaterial* PhysMat, const FName& Type) const
{
	FString SurfaceName = "Default";

	if (PhysMat)
	{
		EPhysicalSurface SurfaceType = PhysMat->SurfaceType;

		switch (SurfaceType)
		{
		case SurfaceType1: SurfaceName = "Stone"; break;
		case SurfaceType2: SurfaceName = "Dirt"; break;
		case SurfaceType3: SurfaceName = "Grass"; break;
		case SurfaceType4: SurfaceName = "Sand"; break;
		case SurfaceType5: SurfaceName = "Metal"; break;
		case SurfaceType6: SurfaceName = "Wood"; break;
		default: SurfaceName = "Default"; break;
		}
	}
	FString TagString = FString::Printf(TEXT("Sound.Movement.Footstep.%s.%s"), *Type.ToString(), *SurfaceName);
	return FGameplayTag::RequestGameplayTag(FName(*TagString), false);
}

FGameplayTag UKRAnimNotify_Footstep::GetEffectTagForSurface(UPhysicalMaterial* PhysMat, const FName& Type) const
{
	FString SurfaceName = "Default";

	if (PhysMat)
	{
		EPhysicalSurface SurfaceType = PhysMat->SurfaceType;

		switch (SurfaceType)
		{
		case SurfaceType1: SurfaceName = "Stone"; break;
		case SurfaceType2: SurfaceName = "Dirt"; break;
		case SurfaceType3: SurfaceName = "Grass"; break;
		case SurfaceType4: SurfaceName = "Sand"; break;
		case SurfaceType5: SurfaceName = "Metal"; break;
		case SurfaceType6: SurfaceName = "Wood"; break;
		default: SurfaceName = "Default"; break;
		}
	}
	FString TagString = FString::Printf(TEXT("Effect.Movement.Footstep.%s.%s"), *Type.ToString(), *SurfaceName);
	return FGameplayTag::RequestGameplayTag(FName(*TagString), false);
}

#if WITH_EDITOR
FString UKRAnimNotify_Footstep::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Footstep: %s (%s)"), *FootstepType.ToString(), *FootSocketName.ToString());
}
#endif
