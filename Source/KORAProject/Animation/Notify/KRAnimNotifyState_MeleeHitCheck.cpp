#include "Animation/Notify/KRAnimNotifyState_MeleeHitCheck.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/KREventTag.h"

UKRAnimNotifyState_MeleeHitCheck::UKRAnimNotifyState_MeleeHitCheck()
{
	// 기본 이벤트 태그 설정
	BeginEventTag = KRTAG_EVENT_MELEE_HITCHECK_BEGIN;
	TickEventTag = KRTAG_EVENT_MELEE_HITCHECK_TICK;
	EndEventTag = KRTAG_EVENT_MELEE_HITCHECK_END;
}

FString UKRAnimNotifyState_MeleeHitCheck::GetNotifyName_Implementation() const
{
	return TEXT("Melee Hit Check");
}

void UKRAnimNotifyState_MeleeHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	SendHitCheckEvent(MeshComp, BeginEventTag);
}

void UKRAnimNotifyState_MeleeHitCheck::NotifyTick(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	SendHitCheckEvent(MeshComp, TickEventTag);
}

void UKRAnimNotifyState_MeleeHitCheck::NotifyEnd(USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	SendHitCheckEvent(MeshComp, EndEventTag);
}

void UKRAnimNotifyState_MeleeHitCheck::SendHitCheckEvent(USkeletalMeshComponent* MeshComp, const FGameplayTag& EventTag)
{
	if (!EventTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetASC(MeshComp);
	if (!ASC)
	{
		return;
	}

	// GameplayEvent를 통해 히트 체크 알림 (특정 GA에 의존하지 않음)
	FGameplayEventData EventData;
	EventData.Instigator = MeshComp ? MeshComp->GetOwner() : nullptr;
	ASC->HandleGameplayEvent(EventTag, &EventData);
}

UAbilitySystemComponent* UKRAnimNotifyState_MeleeHitCheck::GetASC(USkeletalMeshComponent* MeshComp) const
{
	if (!MeshComp)
	{
		return nullptr;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
}
