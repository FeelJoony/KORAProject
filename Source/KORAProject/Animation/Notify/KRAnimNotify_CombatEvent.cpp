#include "KRAnimNotify_CombatEvent.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "SubSystem/KREffectSubsystem.h"

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

	// 이벤트 타입별 처리
	switch (EventType)
	{
	case EKRCombatEventType::PlaySound:
		{
			if (UKRSoundSubsystem* SoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>())
			{
				FVector Location = (SocketName != NAME_None)
					? MeshComp->GetSocketLocation(SocketName)
					: Owner->GetActorLocation();
				SoundSubsystem->PlaySoundByTag(EventTag, Location, Owner);
			}
		}
		break;

	case EKRCombatEventType::SpawnEffect:
		{
			if (UKREffectSubsystem* EffectSubsystem = World->GetSubsystem<UKREffectSubsystem>())
			{
				if (SocketName != NAME_None)
				{
					EffectSubsystem->SpawnEffectAttached(EventTag, MeshComp, SocketName);
				}
				else
				{
					FTransform SpawnTransform = Owner->GetActorTransform();
					EffectSubsystem->SpawnEffectByTag(EventTag, SpawnTransform, Owner);
				}
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
