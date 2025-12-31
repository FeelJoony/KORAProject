#include "Animation/Notify/KRAnimNotifyState_EnemySlash.h"
#include "GAS/Abilities/EnemyAbility/KRGA_EnemySlash.h"
#include "Characters/KREnemyCharacter.h"
#include "Components/KRCombatComponent.h"
#include "Enemy/KRAIC_Enemy.h"
#include "Characters/KRHeroCharacter.h"

void UKRAnimNotifyState_EnemySlash::NotifyBegin(USkeletalMeshComponent* MeshComp, 
												UAnimSequenceBase* Animation, 
												float TotalDuration, 
												const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AKREnemyCharacter* Enemy = Cast<AKREnemyCharacter>(MeshComp->GetOwner());
	if (!IsValid(Enemy)) return;

	AController* AIC = Enemy->GetController();
	if (!AIC) return;
	UE_LOG(LogTemp, Warning, TEXT("Get AIC"));
	
	AKRAIC_Enemy* EnemyAIC = Cast<AKRAIC_Enemy>(AIC);
	if (!EnemyAIC) return;
	UE_LOG(LogTemp, Warning, TEXT("Get Enmey AIC"));

	AActor* Target = EnemyAIC->GetAttackTarget();
	if (!Target) return;
	UE_LOG(LogTemp, Warning, TEXT("Get Target"));

	AKRHeroCharacter* Player = Cast<AKRHeroCharacter>(Target);
	UE_LOG(LogTemp, Warning, TEXT("Get Player"));

	UKRCombatComponent* EnemyCombatComp = Enemy->GetEnemyCombatComponent();
	if (EnemyCombatComp)
	{
		EnemyCombatComp->ApplyDamageToTarget(Player, 10, false, nullptr);
	}
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
