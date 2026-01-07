#include "GAS/Abilities/HeroAbilities/Attack/KRGameplayAbility_RangeLight.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "Item/Weapons/KRRangeWeapon.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"

UKRGameplayAbility_RangeLight::UKRGameplayAbility_RangeLight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKRGameplayAbility_RangeLight::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponentFromActorInfo())
		{
			bool bFound = false;
			float CurrentAmmo = KRASC->GetNumericAttribute(UKRCombatCommonSet::GetCurrentAmmoAttribute()); 
			float MaxAmmo = KRASC->GetNumericAttribute(UKRCombatCommonSet::GetMaxAmmoAttribute());
		}

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);

		return;
	}
	
	
    UAnimMontage* FireMontage = GetMontageFromEquipment(0);

	if (FireMontage)
    {
        UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
            this, FName("FireAnim"), FireMontage, 1.0f, NAME_None, false
        );
		
        Task->ReadyForActivation();
    }

	FRotator AimRot = GetFinalAimRotation(MaxRange);
	
	AKRRangeWeapon* Weapon = GetRangeWeapon();
	if (Weapon)
	{
		FireWeaponActor(AimRot);
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

const TArray<TObjectPtr<UAnimMontage>>* UKRGameplayAbility_RangeLight::GetMontageArrayFromEntry(const FKRAppliedEquipmentEntry& Entry) const
{
	return &Entry.GetLightAttackMontages();
}