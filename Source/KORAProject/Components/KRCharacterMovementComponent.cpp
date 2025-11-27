#include "Components/KRCharacterMovementComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "GameplayTag/KRStateTag.h"

float UKRCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar)
	{
		return MaxSpeed;
	}
	
	if (!CachedASC.IsValid())
	{
		if (UAbilitySystemComponent* BaseASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerChar))
		{
			CachedASC = Cast<UKRAbilitySystemComponent>(BaseASC);
		}
	}
	
	if (CachedASC.IsValid())
	{
		if (CachedASC->HasMatchingGameplayTag(KRTAG_STATE_RESTRICT_INPUT))
		{
			return 0.f;
		}
	}

	return MaxSpeed;
}

void UKRCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* BaseASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()); 
	CachedASC = Cast<UKRAbilitySystemComponent>(BaseASC);
}
