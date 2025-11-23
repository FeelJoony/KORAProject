#include "Components/Combat/HeroCombatComponent.h"
#include "Item/Weapons/KRWeaponBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/KREventTag.h"
#include "KORATypes/KRStructTypes.h"

#include "KORADebugHelper.h"

AKRWeaponBase* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	return Cast<AKRWeaponBase>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}

AKRWeaponBase* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
	return Cast<AKRWeaponBase>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const
{
	AKRWeaponBase* CurrentWeapon = GetHeroCurrentEquippedWeapon();

	if (CurrentWeapon)
	{
		FKRWeaponCommonData CommonData = CurrentWeapon->GetCommonData();

		return CommonData.WeaponBaseDamage.GetValueAtLevel(InLevel);
	}

	return 0.f;
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor, const FHitResult& HitResult)
{
	if (OverlappedActors.Contains(HitActor))
	{
		return;
	}

	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningPawn(), KRTAG_EVENT_COMBAT_HIT, Data);
	
}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor, const FHitResult& HitResult)
{
	
}