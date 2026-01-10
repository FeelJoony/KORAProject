#include "GAS/Abilities/HeroAbilities/KRGA_Reload.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Kismet/GameplayStatics.h"

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

	Step1_StartReload();
}

USoundBase* UKRGA_Reload::GetSoundFromDA(UKRSoundDefinition* InDA)
{
	if (InDA && !InDA->Sound.IsNull())
	{
		return InDA->Sound.LoadSynchronous();
	}
	return nullptr;
}

void UKRGA_Reload::Step1_StartReload()
{
	if (USoundBase* Sound = GetSoundFromDA(DA_ReloadStart))
	{
		UGameplayStatics::SpawnSoundAttached(Sound, GetAvatarActorFromActorInfo()->GetRootComponent());
	}
	
	UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, Delay_Start);
	Task->OnFinish.AddDynamic(this, &UKRGA_Reload::Step2_LoadReload);
	Task->ReadyForActivation();
}

void UKRGA_Reload::Step2_LoadReload()
{
	if (USoundBase* Sound = GetSoundFromDA(DA_ReloadLoad))
	{
		UGameplayStatics::SpawnSoundAttached(Sound, GetAvatarActorFromActorInfo()->GetRootComponent());
	}
	
	UAbilityTask_WaitDelay* Task = UAbilityTask_WaitDelay::WaitDelay(this, Delay_Load);
	Task->OnFinish.AddDynamic(this, &UKRGA_Reload::Step3_EndReload);
	Task->ReadyForActivation();
}

void UKRGA_Reload::Step3_EndReload()
{
	if (USoundBase* Sound = GetSoundFromDA(DA_ReloadEnd))
	{
		UGameplayStatics::SpawnSoundAttached(Sound, GetAvatarActorFromActorInfo()->GetRootComponent());
	}
	
	if (ReloadEffectClass)
	{
		ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, ReloadEffectClass.GetDefaultObject(), 1.0f);
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