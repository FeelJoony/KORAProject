#include "GAS/AbilitySet/KRAbilitySet.h"

#include "AssetTypeCategories.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/Abilities/KRGameplayAbility.h"

void FKRAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FKRAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FKRAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* AttributeSet)
{
	GrantedAttributeSets.Add(AttributeSet);
}

void FKRAbilitySet_GrantedHandles::TakeFromAbilitySystem(UKRAbilitySystemComponent* KRASC)
{
	check(KRASC);

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			KRASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			KRASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* AttributeSet : GrantedAttributeSets)
	{
		KRASC->RemoveSpawnedAttribute(AttributeSet);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UKRAbilitySet::UKRAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UKRAbilitySet::GiveToAbilitySystem(UKRAbilitySystemComponent* KRASC,
                                        FKRAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(KRASC);

	for (const TSubclassOf<UAttributeSet>& SetClass : GrantedAttributeSets)
	{
		if (!IsValid(SetClass)) continue;

		UAttributeSet* NewSet = NewObject<UAttributeSet>(KRASC->GetOwner(), SetClass);
		KRASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}

	for (const FKRAbilitySet_GameplayAbility& AbilityToGrant : GrantedGameplayAbilities)
	{
		if (!IsValid(AbilityToGrant.Ability)) continue;

		UKRGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UKRGameplayAbility>();
		
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = KRASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	for (const FKRAbilitySet_GameplayEffect& EffectToGrant : GrantedGameplayEffects)
	{
		if (!IsValid(EffectToGrant.GameplayEffect)) continue;

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = KRASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, KRASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}
