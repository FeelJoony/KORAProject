#include "Animation/Notify/KRAnimNotify_EndSection.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/KREventTag.h"

UKRAnimNotify_EndSection::UKRAnimNotify_EndSection()
{
	EndSectionEventTag = KRTAG_EVENT_ENEMY_ATTACK_END_SECTION;
}

void UKRAnimNotify_EndSection::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	SendEndSectionEvent(MeshComp);
}

#if WITH_EDITOR
FString UKRAnimNotify_EndSection::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("End Section [%s]"), *EndSectionEventTag.ToString());
}
#endif

void UKRAnimNotify_EndSection::SendEndSectionEvent(USkeletalMeshComponent* MeshComp)
{
	if (!EndSectionEventTag.IsValid()) return;
	UAbilitySystemComponent* ASC = GetASC(MeshComp);
	if (!ASC) return;

	AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;

	FGameplayEventData EventData;
	EventData.Instigator = Owner ? Owner->GetInstigator() : nullptr;
	ASC->HandleGameplayEvent(EndSectionEventTag, &EventData);
}

UAbilitySystemComponent* UKRAnimNotify_EndSection::GetASC(USkeletalMeshComponent* MeshComp) const
{
	if (!MeshComp) return nullptr;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return nullptr;
	
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
}
