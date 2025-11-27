#include "Components/KRCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/KREventTag.h"

UKRCombatComponent::UKRCombatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UKRCombatComponent::ToggleWeaponCollision(bool bShouldEnable)
{
	/*AKRWeaponBase* Weapon = GetCurrentWeapon();
	if (!Weapon) return;

	UPrimitiveComponent* CollisionComp = Weapon->GetWeaponCollisionBox(); 
	if (!CollisionComp) return;

	if (bShouldEnable)
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		OverlappedActors.Empty(); // 히트 목록 초기화
	}
	else
	{
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlappedActors.Empty();
	}*/
}

void UKRCombatComponent::OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult)
{
	if (!HitActor || OverlappedActors.Contains(HitActor))
	{
		return;
	}

	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetPawnChecked<APawn>();
	Data.Target = HitActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawnChecked<APawn>(), KRTAG_EVENT_COMBAT_HIT, Data);
	
}

AKRWeaponBase* UKRCombatComponent::GetCurrentWeapon() const
{
	// 무기 구조 확립 후 EquipmentManager를 통해 찾는 로직 작성 예정
	return nullptr;
}
