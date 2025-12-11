#include "Player/KRPlayerState.h"
#include "GameModes/KRBaseGameMode.h"
#include "AbilitySystemComponent.h"
#include "Components/KRPawnExtensionComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KRPlayerAttributeSet.h"
#include "Data/DataAssets/KRPawnData.h"
#include "GAS/AbilitySet/KRAbilitySet.h"
#include "Components/KRCurrencyComponent.h"
#include "Net/UnrealNetwork.h"

AKRPlayerState::AKRPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetNetUpdateFrequency(100.f);

	KRASC = CreateDefaultSubobject<UKRAbilitySystemComponent>(TEXT("ASC"));
	KRASC->SetIsReplicated(true);
	KRASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CombatCommonSet = CreateDefaultSubobject<UKRCombatCommonSet>(TEXT("CombatCommonSet"));
	PlayerAttributeSet = CreateDefaultSubobject<UKRPlayerAttributeSet>(TEXT("PlayerAttributeSet"));

	CurrencyComponent = CreateDefaultSubobject<UKRCurrencyComponent>(TEXT("CurrencyComponent"));
}

void AKRPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* AKRPlayerState::GetAbilitySystemComponent() const
{
	return KRASC;
}

void AKRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKRPlayerState, PawnData);
}

void AKRPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AKRPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(KRASC)
	KRASC->InitAbilityActorInfo(this, GetPawn());
}

void AKRPlayerState::OnExperienceLoaded(const class UKRExperienceDefinition* CurrentExperience)
{
	if (AKRBaseGameMode* GameMode = GetWorld()->GetAuthGameMode<AKRBaseGameMode>())
	{
		if (const UKRPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
	}
}

void AKRPlayerState::SetPawnData(const UKRPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority) return;

	if (PawnData)
	{
		return;
	}

	PawnData = InPawnData;

	for (const UKRAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(KRASC, nullptr);
		}
	}

	if (APawn* Pawn = GetPawn())
    {
    	if (UKRPawnExtensionComponent* PawnExtComp = UKRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
    	{
    		PawnExtComp->SetPawnData(PawnData);
    	}
    }

	ForceNetUpdate();
}

void AKRPlayerState::OnRep_PawnData()
{
	if (PawnData)
	{
		if (APawn* Pawn = GetPawn())
		{
			if (UKRPawnExtensionComponent* PawnExtComp = UKRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
			{
				PawnExtComp->SetPawnData(PawnData);
			}
		}
	}
}
