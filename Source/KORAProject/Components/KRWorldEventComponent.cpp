#include "KRWorldEventComponent.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "SubSystem/KREffectSubsystem.h"
#include "Components/AudioComponent.h"

UKRWorldEventComponent::UKRWorldEventComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKRWorldEventComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UKRWorldEventComponent::TriggerWorldEvent(const FGameplayTag& EventTag, AActor* Instigator)
{
	UKRWorldEventData* EventData = GetWorldEventData(EventTag);
	if (!EventData)
	{
		return false;
	}

	return TriggerWorldEventFromData(EventData, Instigator);
}

bool UKRWorldEventComponent::TriggerWorldEventFromData(UKRWorldEventData* EventData, AActor* Instigator)
{
	if (!EventData || !CanTriggerEvent(EventData))
	{
		return false;
	}
	
	PlayEventEffects(EventData);
	if (Instigator)
	{
		ApplyGameplayEffects(EventData, Instigator);
	}
	
	int32& TriggerCount = EventTriggerCounts.FindOrAdd(EventData->EventTag);
	TriggerCount++;
	
	UWorld* World = GetWorld();
	if (World)
	{
		LastTriggerTimes.FindOrAdd(EventData->EventTag) = World->GetTimeSeconds();
	}
	
	OnWorldEventTriggered.Broadcast(EventData->EventTag);

	return true;
}

void UKRWorldEventComponent::EndWorldEvent(const FGameplayTag& EventTag)
{
	if (TObjectPtr<UAudioComponent>* FoundAudio = ActiveEventSounds.Find(EventTag))
	{
		if (*FoundAudio)
		{
			(*FoundAudio)->Stop();
		}
		ActiveEventSounds.Remove(EventTag);
	}

	// TODO: 지속 이펙트 정지
	// TODO: Gameplay Effect 제거
	
	UKRWorldEventData* EventData = GetWorldEventData(EventTag);
	if (EventData && EventData->ExitEventTag.IsValid())
	{
		TriggerWorldEvent(EventData->ExitEventTag);
	}
}

void UKRWorldEventComponent::RegisterWorldEventData(UKRWorldEventData* EventData)
{
	if (EventData && EventData->EventTag.IsValid())
	{
		RegisteredEvents.Add(EventData->EventTag, EventData);
	}
}

UKRWorldEventData* UKRWorldEventComponent::GetWorldEventData(const FGameplayTag& EventTag) const
{
	if (const TObjectPtr<UKRWorldEventData>* Found = RegisteredEvents.Find(EventTag))
	{
		return *Found;
	}
	return nullptr;
}

UKRSoundSubsystem* UKRWorldEventComponent::GetSoundSubsystem() const
{
	if (CachedSoundSubsystem.IsValid())
	{
		return CachedSoundSubsystem.Get();
	}

	if (UWorld* World = GetWorld())
	{
		CachedSoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>();
	}
	return CachedSoundSubsystem.Get();
}

UKREffectSubsystem* UKRWorldEventComponent::GetEffectSubsystem() const
{
	if (CachedEffectSubsystem.IsValid())
	{
		return CachedEffectSubsystem.Get();
	}

	if (UWorld* World = GetWorld())
	{
		CachedEffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
	}
	return CachedEffectSubsystem.Get();
}

bool UKRWorldEventComponent::CanTriggerEvent(const UKRWorldEventData* EventData) const
{
	if (!EventData)
	{
		return false;
	}

	const FKRWorldEventTriggerCondition& Condition = EventData->TriggerCondition;
	
	if (Condition.bTriggerOnce)
	{
		const int32* TriggerCount = EventTriggerCounts.Find(EventData->EventTag);
		if (TriggerCount && *TriggerCount > 0)
		{
			return false;
		}
	}
	
	if (Condition.MaxTriggerCount > 0)
	{
		const int32* TriggerCount = EventTriggerCounts.Find(EventData->EventTag);
		if (TriggerCount && *TriggerCount >= Condition.MaxTriggerCount)
		{
			return false;
		}
	}
	
	if (Condition.MinTriggerInterval > 0.0f)
	{
		const float* LastTime = LastTriggerTimes.Find(EventData->EventTag);
		if (LastTime)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				float TimeSinceLastTrigger = World->GetTimeSeconds() - *LastTime;
				if (TimeSinceLastTrigger < Condition.MinTriggerInterval)
				{
					return false;
				}
			}
		}
	}

	// TODO: Required/Blocked Tags 확인

	return true;
}

void UKRWorldEventComponent::PlayEventEffects(const UKRWorldEventData* EventData)
{
	if (!EventData)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const FKRWorldEventEffects& Effects = EventData->Effects;
	FVector Location = Owner->GetActorLocation();
	
	if (UKRSoundSubsystem* SoundSubsystem = GetSoundSubsystem(); SoundSubsystem && Effects.SoundTag.IsValid())
	{
		UAudioComponent* AudioComp = SoundSubsystem->PlaySoundByTag(Effects.SoundTag, Location, Owner, !Effects.bLoopSound);

		if (Effects.bLoopSound && AudioComp)
		{
			ActiveEventSounds.Add(EventData->EventTag, AudioComp);
		}
	}
	
	if (UKREffectSubsystem* EffectSubsystem = GetEffectSubsystem(); EffectSubsystem && Effects.EffectTag.IsValid())
	{
		FTransform SpawnTransform = Owner->GetActorTransform();
		EffectSubsystem->SpawnEffectByTag(Effects.EffectTag, SpawnTransform, Owner);
	}

	// TODO: 카메라 조정
}

void UKRWorldEventComponent::ApplyGameplayEffects(const UKRWorldEventData* EventData, AActor* Target)
{
	if (!EventData || !Target)
	{
		return;
	}

	// TODO: Gameplay Effects 적용 (GAS와 통합)
	// TODO: Gameplay Tags 추가/제거
	// TODO: Gameplay Cue 발동
}
