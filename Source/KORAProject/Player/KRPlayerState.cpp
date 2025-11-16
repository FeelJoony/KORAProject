#include "Player/KRPlayerState.h"
#include "AbilitySystemComponent.h"
#include "GAS/KRPlayerAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "GameFramework/Pawn.h"

AKRPlayerState::AKRPlayerState()
{
	SetNetUpdateFrequency(100.f);
	
	PlayerASC = CreateDefaultSubobject<UKRPlayerAbilitySystemComponent>(TEXT("ASC"));
	PlayerASC->SetIsReplicated(true);
	PlayerASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CombatCommonSet = CreateDefaultSubobject<UKRCombatCommonSet>(TEXT("CombatCommonSet"));
	PlayerAttributeSet = CreateDefaultSubobject<UKRPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
}

void AKRPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* AKRPlayerState::GetAbilitySystemComponent() const
{
	return GetPlayerAbilitySystemComponent();
}


void AKRPlayerState::InitASCForAvatar(AActor* NewAvatar)
{
	if (IsValid(PlayerASC))
	{
		PlayerASC->InitAbilityActorInfo(this, NewAvatar);
	}

	
}
