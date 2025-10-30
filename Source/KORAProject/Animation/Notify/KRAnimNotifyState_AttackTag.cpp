#include "Animation/Notify/KRAnimNotifyState_AttackTag.h"

#include "Characters/KRHeroCharacter.h"

void UKRAnimNotifyState_AttackTag::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (AKRHeroCharacter* Character = Cast<AKRHeroCharacter>(MeshComp->GetOwner()))
	{
		if (AActor* Sword = Character->GetCurrentSword())
		{
			// 태그 추가
			//Sword->ComponentTags.AddUnique(WeaponDamageTag);
			//Sword->오버랩 활성화
		}
	}
}

void UKRAnimNotifyState_AttackTag::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AKRHeroCharacter* Character = Cast<AKRHeroCharacter>(MeshComp->GetOwner()))
	{
		if (AActor* Sword = Character->GetCurrentSword())
		{
			// 태그 제거
			//Sword->ComponentTags.Remove(WeaponDamageTag);
			//Sword->오버랩 취소
		}
	}
}
