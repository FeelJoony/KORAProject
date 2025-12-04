#include "Characters/KRBaseCharacter.h"
#include "Components/KRCombatComponent.h"
#include "Components/KRPawnExtensionComponent.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GAS/KRAbilitySystemComponent.h"

AKRBaseCharacter::AKRBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnExtensionComponent = CreateDefaultSubobject<UKRPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	CombatComponent = CreateDefaultSubobject<UKRCombatComponent>(TEXT("CombatComponent"));
	EquipmentManagerComponent = CreateDefaultSubobject<UKREquipmentManagerComponent>(TEXT("EquipmentManager"));
}

UAbilitySystemComponent* AKRBaseCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtensionComponent)
	{
		return PawnExtensionComponent->GetKRAbilitySystemComponent();
	}
	return nullptr;
}

