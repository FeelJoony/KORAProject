#include "Player/KRPlayerState.h"
#include "GAS/KRPlayerAbilitySystemComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"

AKRPlayerState::AKRPlayerState()
{
	SetNetUpdateFrequency(100.f);
	
	KRPlayerASC = CreateDefaultSubobject<UKRAbilitySystemComponent>(TEXT("ASC"));
	KRPlayerASC->SetIsReplicated(true);
	KRPlayerASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AKRPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* AKRPlayerState::GetAbilitySystemComponent() const
{
	return GetKRPlayerAbilitySystemComponent();
}


void AKRPlayerState::InitASCForAvatar(AActor* NewAvatar)
{
	if (IsValid(KRPlayerASC))
	{
		KRPlayerASC->InitAbilityActorInfo(this, NewAvatar);
	}

	
}
