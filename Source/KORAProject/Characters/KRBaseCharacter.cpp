#include "Characters/KRBaseCharacter.h"

AKRBaseCharacter::AKRBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;
}

UAbilitySystemComponent* AKRBaseCharacter::GetAbilitySystemComponent() const
{
	return CachedASC;
}
