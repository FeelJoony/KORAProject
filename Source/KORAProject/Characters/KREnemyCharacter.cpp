#include "Characters/KREnemyCharacter.h"
#include "Components/Combat/EnemyCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/KRAttributeSet.h"
#include "Engine/AssetManager.h"
#include "Data/StartUpData/DataAsset_EnemyStartUpData.h"

#include "KORADebugHelper.h"

AKREnemyCharacter::AKREnemyCharacter()
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

	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>(TEXT("EnemyCombatComponent"));

	EnemyASC = CreateDefaultSubobject<UKRAbilitySystemComponent>(TEXT("EnemyASC"));
	EnemyASC->SetIsReplicated(true);
	EnemyASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	EnemyAttributeSet = CreateDefaultSubobject<UKRAttributeSet>(TEXT("EnemyAttributeSet"));
}

void AKREnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitEnemyASC();
	InitEnemyStartUpData();
}

void AKREnemyCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	InitEnemyASC();
	InitEnemyStartUpData();
}

void AKREnemyCharacter::InitEnemyASC()
{
	if (!EnemyASC) return;

	if (!EnemyASC->AbilityActorInfo.IsValid())
	{
		EnemyASC->InitAbilityActorInfo(this, this);
		SetCachedASC(EnemyASC);
	}
}

void AKREnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull()) return;

	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
		[this]()
		{
			if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
			{
				LoadedData->GiveToAbilitySystemComponent(EnemyASC);
				Debug::Print(TEXT("Enemy Start Up Data Loaded"), FColor::Green);
			}
		}
		));
}
