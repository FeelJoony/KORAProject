#include "Animation/Notify/KRAnimNotifyState_ApplyTags.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UKRAnimNotifyState_ApplyTags::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerActor);
	if (!ASI) return;
	
	UAbilitySystemComponent* ASC =  ASI->GetAbilitySystemComponent();
	if (!ASC) return;
	
	for (FGameplayTag Tag : GameplayTags)
	{
		ASC->AddLooseGameplayTag(Tag);
	}
}

void UKRAnimNotifyState_ApplyTags::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) return;

	const IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OwnerActor);
	if (!ASI) return;
	
	UAbilitySystemComponent* ASC =  ASI->GetAbilitySystemComponent();
	if (!ASC) return;
	
	for (FGameplayTag Tag : GameplayTags)
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}
