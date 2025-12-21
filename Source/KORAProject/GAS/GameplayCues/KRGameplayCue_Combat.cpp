#include "KRGameplayCue_Combat.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "SubSystem/KREffectSubsystem.h"
#include "GameFramework/Character.h"

UKRGameplayCue_Combat::UKRGameplayCue_Combat()
{
	SoundTagFilter = FGameplayTag::RequestGameplayTag(FName("Sound"), false);
	EffectTagFilter = FGameplayTag::RequestGameplayTag(FName("Effect"), false);
}

bool UKRGameplayCue_Combat::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
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
		if (UKRSoundSubsystem* SoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>())
		{
			SoundSubsystem->PlaySoundByTag(SoundTag, Location, MyTarget);
		}
	}
	
	FGameplayTag EffectTag = FindEffectTagFromParams(Parameters);
	if (!EffectTag.IsValid())
	{
		EffectTag = DefaultEffectTag;
	}

	if (EffectTag.IsValid())
	{
		if (UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>())
		{
			if (DefaultEffectSocketName != NAME_None)
			{
				if (ACharacter* Character = Cast<ACharacter>(MyTarget))
				{
					if (USkeletalMeshComponent* Mesh = Character->GetMesh())
					{
						EffectSubsystem->SpawnEffectAttached(EffectTag, Mesh, DefaultEffectSocketName);
						return true;
					}
				}
			}
			
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(Location);
			if (!Parameters.Normal.IsZero())
			{
				SpawnTransform.SetRotation(Parameters.Normal.ToOrientationQuat());
			}
			EffectSubsystem->SpawnEffectByTag(EffectTag, SpawnTransform, MyTarget);
		}
	}

	return true;
}

FGameplayTag UKRGameplayCue_Combat::FindSoundTagFromParams(const FGameplayCueParameters& Parameters) const
{
	if (!SoundTagFilter.IsValid())
	{
		return FGameplayTag();
	}

	for (const FGameplayTag& Tag : Parameters.AggregatedSourceTags)
	{
		if (Tag.MatchesTag(SoundTagFilter))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

FGameplayTag UKRGameplayCue_Combat::FindEffectTagFromParams(const FGameplayCueParameters& Parameters) const
{
	if (!EffectTagFilter.IsValid())
	{
		return FGameplayTag();
	}

	for (const FGameplayTag& Tag : Parameters.AggregatedSourceTags)
	{
		if (Tag.MatchesTag(EffectTagFilter))
		{
			return Tag;
		}
	}

	return FGameplayTag();
}
