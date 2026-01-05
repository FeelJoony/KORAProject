#include "Animation/Notify/KRAnimNotify_EnemyMeleeHitCheck.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Attack.h"

void UKRAnimNotify_EnemyMeleeHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp)
	{
		return;
	}
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}
	UKRAbilitySystemComponent* KRASC = Cast<UKRAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor));
	if (!KRASC)
	{
		return;
	}
	UKRGA_Enemy_Attack* EnemyAttackGA = KRASC->GetActiveAbilityByClass<UKRGA_Enemy_Attack>();
	if (!EnemyAttackGA)
	{
		return;
	}
	EnemyAttackGA->PerformHitCheck();
}
