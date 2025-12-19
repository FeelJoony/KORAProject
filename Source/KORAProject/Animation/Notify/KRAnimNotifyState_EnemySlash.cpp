#include "Animation/Notify/KRAnimNotifyState_EnemySlash.h"
#include "GAS/Abilities/EnemyAbility/KRGA_EnemySlash.h"
#include "Characters/KREnemyCharacter.h"
#include "Components/KRCombatComponent.h"

void UKRAnimNotifyState_EnemySlash::NotifyBegin(USkeletalMeshComponent* MeshComp, 
												UAnimSequenceBase* Animation, 
												float TotalDuration, 
												const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(MeshComp->GetOwner());
	if (!IsValid(Enemy)) return;

	/*UKRCombatComponent* EnemyCombatComp = Enemy->GetEnemyCombatComponent();
	if (EnemyCombatComp)
	{
		EnemyCombatComp->ToggleWeaponCollision(true, EToggleDamageType::CurrentEquippedWeapon, EWeaponSlot::RightHand);
	}*/
}

void UKRAnimNotifyState_EnemySlash::NotifyEnd(USkeletalMeshComponent* MeshComp, 
												UAnimSequenceBase* Animation, 
												const FAnimNotifyEventReference& EventReference)
{
	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(MeshComp->GetOwner());
	if (!IsValid(Enemy)) return;

	/*UKRCombatComponent* EnemyCombatComp = Enemy->GetEnemyCombatComponent();
	if (EnemyCombatComp)
	{
		EnemyCombatComp->ToggleWeaponCollision(false, EToggleDamageType::CurrentEquippedWeapon, EWeaponSlot::RightHand);
	}*/
}
