#include "Characters/KREnemyCharacter.h"
#include "Components/KRPawnExtensionComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KREnemyAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "KORADebugHelper.h"

AKREnemyCharacter::AKREnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	
	EnemyASC = CreateDefaultSubobject<UKRAbilitySystemComponent>(TEXT("EnemyASC"));
	EnemyASC->SetIsReplicated(true);
	EnemyASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CombatCommonSet = CreateDefaultSubobject<UKRCombatCommonSet>(TEXT("CombatCommonSet"));
	EnemyAttributeSet = CreateDefaultSubobject<UKREnemyAttributeSet>(TEXT("EnemyAttributeSet"));

	SetNetUpdateFrequency(100.0f);
}

UAbilitySystemComponent* AKREnemyCharacter::GetAbilitySystemComponent() const
{
	return EnemyASC;
}

void AKREnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (PawnExtensionComponent && EnemyASC)
	{
		if (PawnData)
		{
			PawnExtensionComponent->SetPawnData(PawnData);
		}

		PawnExtensionComponent->InitializeAbilitySystem(EnemyASC, this);
	}
}

void AKREnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (PawnExtensionComponent)
	{
		PawnExtensionComponent->HandleControllerChanged();
	}
}
