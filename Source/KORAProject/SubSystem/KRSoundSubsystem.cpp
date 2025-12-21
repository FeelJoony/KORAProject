#include "KRSoundSubsystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void UKRSoundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	MasterVolume = 1.0f;
}

void UKRSoundSubsystem::Deinitialize()
{
	StopAllSounds(0.0f);
	Super::Deinitialize();
}

void UKRSoundSubsystem::Tick(float DeltaTime)
{
	CleanupInactiveSounds();
}

TStatId UKRSoundSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UKRSoundSubsystem, STATGROUP_Tickables);
}

UAudioComponent* UKRSoundSubsystem::PlaySoundByTag(const FGameplayTag& SoundTag, const FVector& Location, AActor* Owner, bool bAutoDestroy)
{
	UKRSoundDefinition* SoundDef = GetSoundDefinition(SoundTag);
	if (!SoundDef)
	{
		return nullptr;
	}

	return PlaySound(SoundDef, Location, Owner, bAutoDestroy);
}

UAudioComponent* UKRSoundSubsystem::PlaySound(const UKRSoundDefinition* SoundDef, const FVector& Location, AActor* Owner, bool bAutoDestroy)
{
	if (!SoundDef || !CanPlaySound(SoundDef->SoundTag))
	{
		return nullptr;
	}

	USoundBase* Sound = SoundDef->Sound.LoadSynchronous();
	if (!Sound)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	
	FVector ListenerLocation = FVector::ZeroVector;
	APlayerController* PC = World->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		ListenerLocation = PC->GetPawn()->GetActorLocation();
	}

	float FinalVolume = CalculateFinalVolume(SoundDef, Location, ListenerLocation);
	float FinalPitch = CalculateRandomPitch(SoundDef);

	UAudioComponent* AudioComponent = CreateAndConfigureAudioComponent(SoundDef, Location, Owner, FinalVolume, FinalPitch);

	if (AudioComponent)
	{
		AudioComponent->bAutoDestroy = bAutoDestroy;
		AudioComponent->Play();

		FKRActiveSoundInstance Instance;
		Instance.AudioComponent = AudioComponent;
		Instance.SoundTag = SoundDef->SoundTag;
		Instance.StartTime = World->GetTimeSeconds();
		Instance.Priority = SoundDef->PlayCondition.Priority;
		ActiveSounds.Add(Instance);

		AddCooldown(SoundDef->SoundTag, SoundDef->PlayCondition.CooldownDuration);
	}

	return AudioComponent;
}

void UKRSoundSubsystem::StopSoundByTag(const FGameplayTag& SoundTag, float FadeOutDuration)
{
	for (int32 i = ActiveSounds.Num() - 1; i >= 0; --i)
	{
		if (ActiveSounds[i].SoundTag.MatchesTagExact(SoundTag) && ActiveSounds[i].IsValid())
		{
			if (FadeOutDuration > 0.0f)
			{
				ActiveSounds[i].AudioComponent->FadeOut(FadeOutDuration, 0.0f);
			}
			else
			{
				ActiveSounds[i].AudioComponent->Stop();
			}
		}
	}
}

void UKRSoundSubsystem::StopAllSounds(float FadeOutDuration)
{
	for (FKRActiveSoundInstance& Instance : ActiveSounds)
	{
		if (Instance.IsValid())
		{
			if (FadeOutDuration > 0.0f)
			{
				Instance.AudioComponent->FadeOut(FadeOutDuration, 0.0f);
			}
			else
			{
				Instance.AudioComponent->Stop();
			}
		}
	}
	ActiveSounds.Empty();
}

void UKRSoundSubsystem::StopSoundsByCategory(const FGameplayTagContainer& CategoryTags, float FadeOutDuration)
{
	for (int32 i = ActiveSounds.Num() - 1; i >= 0; --i)
	{
		if (ActiveSounds[i].IsValid())
		{
			UKRSoundDefinition* SoundDef = GetSoundDefinition(ActiveSounds[i].SoundTag);
			if (SoundDef && SoundDef->CategoryTags.HasAny(CategoryTags))
			{
				if (FadeOutDuration > 0.0f)
				{
					ActiveSounds[i].AudioComponent->FadeOut(FadeOutDuration, 0.0f);
				}
				else
				{
					ActiveSounds[i].AudioComponent->Stop();
				}
			}
		}
	}
}

bool UKRSoundSubsystem::IsSoundPlaying(const FGameplayTag& SoundTag) const
{
	for (const FKRActiveSoundInstance& Instance : ActiveSounds)
	{
		if (Instance.SoundTag.MatchesTagExact(SoundTag) && Instance.IsValid() && Instance.AudioComponent->IsPlaying())
		{
			return true;
		}
	}
	return false;
}

bool UKRSoundSubsystem::CanPlaySound(const FGameplayTag& SoundTag) const
{
	const UKRSoundDefinition* SoundDef = GetSoundDefinition(SoundTag);
	if (!SoundDef)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound Def Null"));
		return false;
	}
	
	if (IsOnCooldown(SoundTag))
	{
		return false;
	}
	
	int32 ActiveCount = GetActiveInstanceCount(SoundTag);
	if (ActiveCount >= SoundDef->PlayCondition.MaxConcurrentInstances)
	{
		return false;
	}

	return true;
}

void UKRSoundSubsystem::RegisterSoundDefinition(UKRSoundDefinition* SoundDef)
{
	if (SoundDef && SoundDef->SoundTag.IsValid())
	{
		SoundDefinitions.Add(SoundDef->SoundTag, SoundDef);
	}
}

UKRSoundDefinition* UKRSoundSubsystem::GetSoundDefinition(const FGameplayTag& SoundTag) const
{
	if (const TObjectPtr<UKRSoundDefinition>* Found = SoundDefinitions.Find(SoundTag))
	{
		return *Found;
	}
	return nullptr;
}

void UKRSoundSubsystem::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UKRSoundSubsystem::SetCategoryVolume(const FGameplayTag& CategoryTag, float Volume)
{
	CategoryVolumeMultipliers.Add(CategoryTag, FMath::Clamp(Volume, 0.0f, 1.0f));
}

void UKRSoundSubsystem::CleanupInactiveSounds()
{
	for (int32 i = ActiveSounds.Num() - 1; i >= 0; --i)
	{
		if (!ActiveSounds[i].IsValid() || !ActiveSounds[i].AudioComponent->IsPlaying())
		{
			ActiveSounds.RemoveAtSwap(i);
		}
	}
}

bool UKRSoundSubsystem::IsOnCooldown(const FGameplayTag& SoundTag) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	float CurrentTime = World->GetTimeSeconds();

	for (const FKRSoundCooldownInfo& Cooldown : SoundCooldowns)
	{
		if (Cooldown.SoundTag.MatchesTagExact(SoundTag))
		{
			if (CurrentTime - Cooldown.LastPlayTime < Cooldown.CooldownDuration)
			{
				return true;
			}
		}
	}

	return false;
}

void UKRSoundSubsystem::AddCooldown(const FGameplayTag& SoundTag, float Duration)
{
	if (Duration <= 0.0f)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	for (FKRSoundCooldownInfo& Cooldown : SoundCooldowns)
	{
		if (Cooldown.SoundTag.MatchesTagExact(SoundTag))
		{
			Cooldown.LastPlayTime = World->GetTimeSeconds();
			Cooldown.CooldownDuration = Duration;
			return;
		}
	}

	FKRSoundCooldownInfo NewCooldown;
	NewCooldown.SoundTag = SoundTag;
	NewCooldown.LastPlayTime = World->GetTimeSeconds();
	NewCooldown.CooldownDuration = Duration;
	SoundCooldowns.Add(NewCooldown);
}

int32 UKRSoundSubsystem::GetActiveInstanceCount(const FGameplayTag& SoundTag) const
{
	int32 Count = 0;
	for (const FKRActiveSoundInstance& Instance : ActiveSounds)
	{
		if (Instance.SoundTag.MatchesTagExact(SoundTag) && Instance.IsValid() && Instance.AudioComponent->IsPlaying())
		{
			++Count;
		}
	}
	return Count;
}

float UKRSoundSubsystem::CalculateFinalVolume(const UKRSoundDefinition* SoundDef, const FVector& Location, const FVector& ListenerLocation) const
{
	if (!SoundDef)
	{
		return 0.0f;
	}

	float Volume = SoundDef->VolumeSettings.BaseVolume;
	
	for (const FGameplayTag& CategoryTag : SoundDef->CategoryTags)
	{
		if (const float* CategoryVolume = CategoryVolumeMultipliers.Find(CategoryTag))
		{
			Volume *= *CategoryVolume;
		}
	}

	float Distance = FVector::Dist(Location, ListenerLocation);

	if (Distance < SoundDef->AttenuationStartDistance)
	{
		Volume *= 1.0f;
	}
	else if (Distance > SoundDef->MaxAudibleDistance)
	{
		Volume = 0.0f;
	}
	else
	{
		float NormalizedDistance = (Distance - SoundDef->AttenuationStartDistance) / (SoundDef->MaxAudibleDistance - SoundDef->AttenuationStartDistance);

		if (SoundDef->VolumeSettings.VolumeDistanceCurve.GetRichCurveConst())
		{
			float CurveValue = SoundDef->VolumeSettings.VolumeDistanceCurve.GetRichCurveConst()->Eval(NormalizedDistance);
			Volume *= FMath::Clamp(CurveValue, 0.0f, 1.0f);
		}
		else
		{
			Volume *= (1.0f - NormalizedDistance);
		}
	}
	
	Volume *= MasterVolume;
	return FMath::Clamp(Volume, 0.0f, 1.0f);
}

float UKRSoundSubsystem::CalculateRandomPitch(const UKRSoundDefinition* SoundDef) const
{
	if (!SoundDef)
	{
		return 1.0f;
	}

	float BasePitch = SoundDef->VolumeSettings.BasePitch;
	FVector2D PitchRange = SoundDef->VolumeSettings.PitchModulationRange;

	return FMath::FRandRange(BasePitch * PitchRange.X, BasePitch * PitchRange.Y);
}

UAudioComponent* UKRSoundSubsystem::CreateAndConfigureAudioComponent(const UKRSoundDefinition* SoundDef, const FVector& Location, AActor* Owner, float Volume, float Pitch)
{
	UWorld* World = GetWorld();
	if (!World || !SoundDef)
	{
		return nullptr;
	}

	USoundBase* Sound = SoundDef->Sound.LoadSynchronous();
	if (!Sound)
	{
		return nullptr;
	}

	UAudioComponent* AudioComponent = NewObject<UAudioComponent>(Owner ? Owner : (UObject*)World);
	if (AudioComponent)
	{
		AudioComponent->SetSound(Sound);
		AudioComponent->SetVolumeMultiplier(Volume);
		AudioComponent->SetPitchMultiplier(Pitch);
		AudioComponent->SetWorldLocation(Location);
		AudioComponent->bAllowSpatialization = true;
		AudioComponent->AttenuationSettings = nullptr;
		AudioComponent->RegisterComponent();
	}

	return AudioComponent;
}
