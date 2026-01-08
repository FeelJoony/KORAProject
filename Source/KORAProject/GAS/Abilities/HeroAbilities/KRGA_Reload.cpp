#include "GAS/Abilities/HeroAbilities/KRGA_Reload.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

UKRGA_Reload::UKRGA_Reload(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UKRGA_Reload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	
	if (UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponentFromActorInfo())
	{
		bool bFound = false;
		float CurrentAmmo = KRASC->GetNumericAttribute(UKRPlayerAttributeSet::GetCurrentAmmoAttribute());
		float MaxAmmo = KRASC->GetNumericAttribute(UKRPlayerAttributeSet::GetMaxAmmoAttribute());

		if (CurrentAmmo >= MaxAmmo)
		{
			return false;
		}
	}
	
	return true;
}

void UKRGA_Reload::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (ReloadEffectClass)
	{
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, ReloadEffectClass.GetDefaultObject(), 1.0f);
		UE_LOG(LogTemp, Log, TEXT(">> [KRGA_Reload] Instant Reload Applied."));
	}
	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(">> [KRGA_Reload] ReloadEffectClass is NULL! Check Blueprint."));
	}

	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (UKREquipmentManagerComponent* EMC = Avatar->FindComponentByClass<UKREquipmentManagerComponent>())
		{
			FKRUIMessage_Weapon Msg;
			Msg.Action = EWeaponMessageAction::AmmoUpdated;
			Msg.WeaponTypeTag = EMC->GetActiveWeaponTypeTag();

			if (UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponentFromActorInfo())
			{
				bool bFound = false;
				float CurrentAmmo = KRASC->GetNumericAttribute(UKRPlayerAttributeSet::GetCurrentAmmoAttribute());
				float MaxAmmo = KRASC->GetNumericAttribute(UKRPlayerAttributeSet::GetMaxAmmoAttribute());

				Msg.CurrentAmmo = (int32)CurrentAmmo;
				Msg.MaxAmmo = (int32)MaxAmmo;
			}
			
			UGameplayMessageSubsystem::Get(this).BroadcastMessage(FKRUIMessageTags::Weapon(), Msg);
		}
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}