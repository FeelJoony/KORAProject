#include "Player/KRPlayerState.h"
#include "GAS/KRPlayerAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"

AKRPlayerState::AKRPlayerState()
{
	KRPlayerASC = CreateDefaultSubobject<UKRPlayerAbilitySystemComponent>(TEXT("ASC"));
	KRPlayerASC->SetIsReplicated(true);
	KRPlayerASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

UAbilitySystemComponent* AKRPlayerState::GetAbilitySystemComponent() const
{
	return KRPlayerASC;
}


void AKRPlayerState::InitializeAbilitySystemForPawn(APawn* NewPawn)
{
	if (!KRPlayerASC) return;

	KRPlayerASC->InitAbilityActorInfo(this, NewPawn);

	if (HasAuthority() && !bStartupGiven)
	{
		KRPlayerASC->ApplyInitialEffects();
		KRPlayerASC->GiveInitialAbilities();
		KRPlayerASC->GiveInputAbilities();
		bStartupGiven = true;
	}
}
