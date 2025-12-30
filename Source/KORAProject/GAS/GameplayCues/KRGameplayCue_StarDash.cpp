#include "KRGameplayCue_StarDash.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "SubSystem/KREffectSubsystem.h"
#include "Data/DataAssets/KREffectDefinition.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTag/KRGameplayCueTag.h"

UKRGameplayCue_StarDash::UKRGameplayCue_StarDash()
{
}

bool UKRGameplayCue_StarDash::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
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

FGameplayTag UKRGameplayCue_StarDash::FindSoundTagFromParams(const FGameplayCueParameters& Parameters) const
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

FGameplayTag UKRGameplayCue_StarDash::FindEffectTagFromParams(const FGameplayCueParameters& Parameters) const
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

void UKRGameplayCue_StarDash::PlaySound(UWorld* World, const FGameplayTag& SoundTag, const FVector& Location, AActor* Target) const
{
	if (UKRSoundSubsystem* SoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>())
	{
		SoundSubsystem->PlaySoundByTag(SoundTag, Location, Target);
	}
}

void UKRGameplayCue_StarDash::SpawnEffect(UWorld* World, const FGameplayTag& EffectTag, AActor* Target, const FVector& FallbackLocation) const
{
	UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	if (!EffectSubsystem)
	{
		return;
	}

	UKREffectDefinition* EffectDef = EffectSubsystem->GetEffectDefinition(EffectTag);
	if (!EffectDef)
	{
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

UKRGameplayCue_StarDash_Start::UKRGameplayCue_StarDash_Start()
{
	GameplayCueTag = KRTAG_GAMEPLAYCUE_STARDASH_START;
}

UKRGameplayCue_StarDash_Dash::UKRGameplayCue_StarDash_Dash()
{
	GameplayCueTag = KRTAG_GAMEPLAYCUE_STARDASH_DASH;
}

UKRGameplayCue_StarDash_Slash::UKRGameplayCue_StarDash_Slash()
{
	GameplayCueTag = KRTAG_GAMEPLAYCUE_STARDASH_SLASH;
}

UKRGameplayCue_StarDash_Finish::UKRGameplayCue_StarDash_Finish()
{
	GameplayCueTag = KRTAG_GAMEPLAYCUE_STARDASH_FINISH;
}
