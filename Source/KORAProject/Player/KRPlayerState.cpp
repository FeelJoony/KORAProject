#include "Player/KRPlayerState.h"
#include "GameModes/KRBaseGameMode.h"
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

	PlayerASC->AddAttributeSetSubobject(CombatCommonSet.Get());
	PlayerASC->AddAttributeSetSubobject(PlayerAttributeSet.Get());
}

void AKRPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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

void AKRPlayerState::OnExperienceLoaded(const class UKRExperienceDefinition* CurrentExperience)
{
	if (AKRBaseGameMode* GameMode = GetWorld()->GetAuthGameMode<AKRBaseGameMode>())
	{
		//const UKRPawnData* NewPawnData = GameMode->GetPawnData
	}
}

void AKRPlayerState::SetPawnData(const class UKRPawnData* NewPawnData)
{
	check(NewPawnData);
	check(!PawnData);

	PawnData = NewPawnData;
}
