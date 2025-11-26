#include "Components/KRCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "GameplayTag/KRStateTag.h"

float UKRCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return MaxSpeed;
	}
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerCharacter))
	{
		if (ASC->HasMatchingGameplayTag(KRTAG_STATE_RESTRICT_INPUT))
		{
			return 0.f;
		}
	}

	return MaxSpeed;
}
