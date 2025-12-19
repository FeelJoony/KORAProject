#include "Components/KRCombatComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "Item/Weapons/KRMeleeWeapon.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "GameplayTag/KREventTag.h"

UKRCombatComponent::UKRCombatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UKRCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		ASC = Cast<UKRAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner));
		EquipmentManager = Owner->FindComponentByClass<UKREquipmentManagerComponent>();
	}
}

AKRWeaponBase* UKRCombatComponent::GetCurrentActiveWeapon() const
{
	if (AKRMeleeWeapon* Melee = EquipmentManager->GetMeleeActorInstance())
	{
		if (!Melee->IsHidden())
		{
			return Melee;
		}
	}

	if (AKRRangeWeapon* Range = EquipmentManager->GetRangeActorInstance())
	{
		if (!Range->IsHidden())
		{
			return Range;
		}
	}
	
	return nullptr;
}

void UKRCombatComponent::HandleMeleeHit(AActor* HitActor, const FHitResult& Hit)
{
	if (!HitActor || !ASC) return;

	ApplyDamageToTarget(HitActor, 0.f, false, &Hit);
}

bool UKRCombatComponent::FireRangedWeapon()
{
	if (!EquipmentManager) return false;

	if (AKRRangeWeapon* RangeWeapon = EquipmentManager->GetRangeActorInstance())
	{
		if (!RangeWeapon->IsHidden())
		{
			RangeWeapon->FireProjectile();
			return true;
		}
	}
	return false;
}

void UKRCombatComponent::ApplyDamageToTarget(AActor* TargetActor, float BaseDamage, bool bIsCritical,
	const FHitResult* HitResult)
{
	if (!ASC || !TargetActor) return;

	FGameplayEventData EventData;
	EventData.Instigator = GetOwner();
	EventData.Target = TargetActor;
	EventData.EventMagnitude = BaseDamage;
	EventData.ContextHandle = ASC->MakeEffectContext();
	if (HitResult)
	{
		EventData.ContextHandle.AddHitResult(*HitResult);
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), KRTAG_EVENT_COMBAT_HIT, EventData);
}
