#include "Animation/Notify/KRAnimNotifyState_SurgeDashHitCheck.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/KREventTag.h"

UKRAnimNotifyState_SurgeDashHitCheck::UKRAnimNotifyState_SurgeDashHitCheck()
{
	HitCheckInterval = 0.05f;
	// 기본 히트 체크 이벤트 태그 설정
	HitCheckEventTag = KRTAG_EVENT_COREDRIVE_DASH_HITCHECK;
}

void UKRAnimNotifyState_SurgeDashHitCheck::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	TimeSinceLastCheck = 0.0f;

	// 시작 시 즉시 한 번 이벤트 발송
	SendHitCheckEvent(MeshComp);
}

void UKRAnimNotifyState_SurgeDashHitCheck::NotifyTick(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	TimeSinceLastCheck += FrameDeltaTime;

	if (TimeSinceLastCheck >= HitCheckInterval)
	{
		TimeSinceLastCheck = 0.0f;
		SendHitCheckEvent(MeshComp);
	}
}

void UKRAnimNotifyState_SurgeDashHitCheck::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	// 종료 시 마지막 이벤트 발송
	SendHitCheckEvent(MeshComp);
}

void UKRAnimNotifyState_SurgeDashHitCheck::SendHitCheckEvent(USkeletalMeshComponent* MeshComp)
{
	if (!HitCheckEventTag.IsValid())
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
	ASC->HandleGameplayEvent(HitCheckEventTag, &EventData);
}

UAbilitySystemComponent* UKRAnimNotifyState_SurgeDashHitCheck::GetASC(USkeletalMeshComponent* MeshComp) const
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
