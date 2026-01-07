#include "Animation/Notify/KRAnimNotifyState_EnemySlash.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/KREventTag.h"

UKRAnimNotifyState_EnemySlash::UKRAnimNotifyState_EnemySlash()
{
	// 기본 히트 체크 이벤트 태그 설정
	HitCheckEventTag = KRTAG_EVENT_ENEMY_SLASH_HITCHECK;
}

FString UKRAnimNotifyState_EnemySlash::GetNotifyName_Implementation() const
{
	return TEXT("Enemy Slash Hit Check");
}

void UKRAnimNotifyState_EnemySlash::NotifyBegin(USkeletalMeshComponent* MeshComp,
												UAnimSequenceBase* Animation,
												float TotalDuration,
												const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// GameplayEvent 발송 (특정 캐릭터/컨트롤러에 의존하지 않음)
	SendHitCheckEvent(MeshComp);
}

void UKRAnimNotifyState_EnemySlash::NotifyEnd(USkeletalMeshComponent* MeshComp,
												UAnimSequenceBase* Animation,
												const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	// 종료 시 별도 처리 없음
}

void UKRAnimNotifyState_EnemySlash::SendHitCheckEvent(USkeletalMeshComponent* MeshComp)
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

UAbilitySystemComponent* UKRAnimNotifyState_EnemySlash::GetASC(USkeletalMeshComponent* MeshComp) const
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
