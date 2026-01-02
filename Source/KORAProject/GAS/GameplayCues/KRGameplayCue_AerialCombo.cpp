#include "KRGameplayCue_AerialCombo.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "SubSystem/KREffectSubsystem.h"
#include "Data/DataAssets/KREffectDefinition.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTag/KRGameplayCueTag.h"

UKRGameplayCue_AerialCombo::UKRGameplayCue_AerialCombo()
{
}

bool UKRGameplayCue_AerialCombo::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!MyTarget)
	{
		return false;
	}

	UWorld* World = MyTarget->GetWorld();
	if (!World)
	{
		return false;
	}

	FVector Location = Parameters.Location.IsZero() ? MyTarget->GetActorLocation() : FVector(Parameters.Location);
	
	FGameplayTag SoundTag = FindSoundTagFromParams(Parameters);
	if (!SoundTag.IsValid())
	{
		SoundTag = DefaultSoundTag;
	}
	if (SoundTag.IsValid())
	{
		PlaySound(World, SoundTag, Location, MyTarget);
	}
	
	FGameplayTag EffectTag = FindEffectTagFromParams(Parameters);
	if (!EffectTag.IsValid())
	{
		EffectTag = DefaultEffectTag;
	}
	if (EffectTag.IsValid())
	{
		SpawnEffect(World, EffectTag, MyTarget, Location);
	}

	return true;
}

FGameplayTag UKRGameplayCue_AerialCombo::FindSoundTagFromParams(const FGameplayCueParameters& Parameters) const
{
	FGameplayTag SoundFilter = FGameplayTag::RequestGameplayTag(FName("Sound"), false);

	for (const FGameplayTag& Tag : Parameters.AggregatedSourceTags)
	{
		if (SoundFilter.IsValid() && Tag.MatchesTag(SoundFilter))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

FGameplayTag UKRGameplayCue_AerialCombo::FindEffectTagFromParams(const FGameplayCueParameters& Parameters) const
{
	FGameplayTag EffectFilter = FGameplayTag::RequestGameplayTag(FName("Effect"), false);

	for (const FGameplayTag& Tag : Parameters.AggregatedSourceTags)
	{
		if (EffectFilter.IsValid() && Tag.MatchesTag(EffectFilter))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

void UKRGameplayCue_AerialCombo::PlaySound(UWorld* World, const FGameplayTag& SoundTag, const FVector& Location, AActor* Target) const
{
	if (UKRSoundSubsystem* SoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>())
	{
		SoundSubsystem->PlaySoundByTag(SoundTag, Location, Target);
	}
}

void UKRGameplayCue_AerialCombo::SpawnEffect(UWorld* World, const FGameplayTag& EffectTag, AActor* Target, const FVector& FallbackLocation) const
{
	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (!EffectSubsystem)
	{
		return;
	}
	
	UKREffectDefinition* EffectDef = EffectSubsystem->GetEffectDefinition(EffectTag);
	if (!EffectDef)
	{
		UE_LOG(LogTemp, Warning, TEXT("AerialCombo Cue: EffectDefinition not found for tag %s"), *EffectTag.ToString());
		return;
	}

	const FKREffectSpawnSettings& SpawnSettings = EffectDef->SpawnSettings;
	if (SpawnSettings.bAttachToParent && SpawnSettings.AttachSocketName != NAME_None)
	{
		if (ACharacter* Character = Cast<ACharacter>(Target))
		{
			if (USkeletalMeshComponent* Mesh = Character->GetMesh())
			{
				EffectSubsystem->SpawnEffectAttached(
					EffectTag,
					Mesh,
					SpawnSettings.AttachSocketName,
					SpawnSettings.LocationOffset,
					SpawnSettings.RotationOffset
				);
				return;
			}
		}
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(FallbackLocation + SpawnSettings.LocationOffset);
	EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, Target);
}

UKRGameplayCue_AerialCombo_Launch::UKRGameplayCue_AerialCombo_Launch()
{
	GameplayCueTag = KRTAG_GAMEPLAYCUE_AERIALCOMBO_LAUNCH;
}

UKRGameplayCue_AerialCombo_Hit::UKRGameplayCue_AerialCombo_Hit()
{
	GameplayCueTag = KRTAG_GAMEPLAYCUE_AERIALCOMBO_HIT;
}

UKRGameplayCue_AerialCombo_Finish::UKRGameplayCue_AerialCombo_Finish()
{
	GameplayCueTag = KRTAG_GAMEPLAYCUE_AERIALCOMBO_FINISH;
}

bool UKRGameplayCue_AerialCombo_Finish::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	Super::OnExecute_Implementation(MyTarget, Parameters);

	if (!MyTarget)
	{
		return false;
	}

	UWorld* World = MyTarget->GetWorld();
	if (!World)
	{
		return false;
	}
	
	if (GroundCrackEffectTag.IsValid())
	{
		FVector Location = Parameters.Location.IsZero() ? MyTarget->GetActorLocation() : FVector(Parameters.Location);

		FVector GroundLocation = Location;
		FHitResult HitResult;
		FVector TraceStart = Location + FVector(0, 0, 50.0f);
		FVector TraceEnd = Location - FVector(0, 0, 200.0f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyTarget);

		if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			GroundLocation = HitResult.Location;
		}

		SpawnEffect(World, GroundCrackEffectTag, MyTarget, GroundLocation);
	}

	return true;
}