#include "KRAnimNotify_CombatEvent.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "SubSystem/KREffectSubsystem.h"
#include "Data/DataAssets/KREffectDefinition.h"

void UKRAnimNotify_CombatEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	UWorld* World = MeshComp->GetWorld();
	if (!World)
	{
		return;
	}

	if (!EventTag.IsValid())
	{
		return;
	}

	switch (EventType)
	{
	case EKRCombatEventType::PlaySound:
		{
			if (UKRSoundSubsystem* SoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>())
			{
				FVector Location = Owner->GetActorLocation();
				SoundSubsystem->PlaySoundByTag(EventTag, Location, Owner);
			}
		}
		break;

	case EKRCombatEventType::SpawnEffect:
		{
			UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>();
			if (!EffectSubsystem)
			{
				break;
			}

			UKREffectDefinition* EffectDef = EffectSubsystem->GetEffectDefinition(EventTag);
			if (!EffectDef)
			{
				UE_LOG(LogTemp, Warning, TEXT("CombatEvent: EffectDefinition not found for tag %s"), *EventTag.ToString());
				break;
			}

			const FKREffectSpawnSettings& SpawnSettings = EffectDef->SpawnSettings;
			FName SocketToUse = NAME_None;
			if (SpawnSettings.bAttachToParent)
			{
				SocketToUse = SpawnSettings.AttachSocketName;
			}

			if (SocketToUse != NAME_None)
			{
				EffectSubsystem->SpawnEffectAttached(
					EventTag,
					MeshComp,
					SocketToUse,
					SpawnSettings.LocationOffset,
					SpawnSettings.RotationOffset
				);
			}
			else
			{
				FTransform SpawnTransform = Owner->GetActorTransform();
				SpawnTransform.SetLocation(SpawnTransform.GetLocation() + SpawnSettings.LocationOffset);
				EffectSubsystem->SpawnEffectByTag(EventTag, SpawnTransform, Owner);
			}
		}
		break;
	}
}

#if WITH_EDITOR
FString UKRAnimNotify_CombatEvent::GetNotifyName_Implementation() const
{
	const FString EventTypeName = (EventType == EKRCombatEventType::PlaySound) ? TEXT("Sound") : TEXT("Effect");
	return FString::Printf(TEXT("Combat: %s [%s]"), *EventTypeName, *EventTag.ToString());
}
#endif
