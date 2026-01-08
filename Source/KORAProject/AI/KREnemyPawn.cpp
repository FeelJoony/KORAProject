#include "KREnemyPawn.h"

#include "KRAIEnemyController.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/KRCombatComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Data/EnemyAbilityDataStruct.h"
#include "Data/EnemyAttributeDataStruct.h"
#include "Data/EnemyDataStruct.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KREnemyAttributeSet.h"
#include "Subsystem/KRDataTablesSubsystem.h"


AKREnemyPawn::AKREnemyPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AKRAIEnemyController::StaticClass();

	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCylinder"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh0"));
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	EnemyASC = CreateDefaultSubobject<UKRAbilitySystemComponent>(TEXT("EnemyASC"));
	CombatComponent = CreateDefaultSubobject<UKRCombatComponent>(TEXT("CombatComponent"));
	
	SetRootComponent(CapsuleComponent);
	Mesh->SetupAttachment(CapsuleComponent);
	ArrowComponent->SetupAttachment(CapsuleComponent);
	
	CombatCommonSet = CreateDefaultSubobject<UKRCombatCommonSet>(TEXT("CombatCommonSet"));
	EnemyAttributeSet = CreateDefaultSubobject<UKREnemyAttributeSet>(TEXT("EnemyAttributeSet"));
}

UAbilitySystemComponent* AKREnemyPawn::GetAbilitySystemComponent() const
{
	return EnemyASC;
}

void AKREnemyPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializeComponents();
}

void AKREnemyPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AKREnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKREnemyPawn::InitializeComponents()
{
	if (EnemyASC == nullptr)
	{
		return;
	}

	EnemyASC->InitAbilityActorInfo(this, this);
	
	if (!EnemyTag.IsValid())
	{
		return;
	}

	const FEnemyDataStruct* EnemyDataStruct = nullptr;
	const FEnemyAbilityDataStruct* EnemyAbilityDataStruct = nullptr;
	const FEnemyAttributeDataStruct* EnemyAttributeDataStruct = nullptr;

	UWorld* World = GetWorld();
	if (World == nullptr || World->GetGameInstance() == nullptr)
	{
		static const FString TablePath = TEXT("/Game/Data/DataTables");

		static const FString EnemyTableName = TEXT("EnemyData");
		static const FString EnemyAbilityTableName = TEXT("EnemyAbilityData");
		static const FString EnemyAttributeTableName = TEXT("EnemyAttributeData");

		EnemyDataStruct = FindDataByLoad<FEnemyDataStruct>(FPaths::Combine(TablePath, EnemyTableName));
		EnemyAbilityDataStruct = FindDataByLoad<FEnemyAbilityDataStruct>(FPaths::Combine(TablePath, EnemyAbilityTableName));
		EnemyAttributeDataStruct = FindDataByLoad<FEnemyAttributeDataStruct>(FPaths::Combine(TablePath, EnemyAttributeTableName));
	}
	else
	{
		UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(this);
		
		EnemyDataStruct = DataTablesSubsystem.GetData<FEnemyDataStruct>(EnemyTag);
		EnemyAbilityDataStruct = DataTablesSubsystem.GetData<FEnemyAbilityDataStruct>(EnemyTag);
		EnemyAttributeDataStruct = DataTablesSubsystem.GetData<FEnemyAttributeDataStruct>(EnemyTag);
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : ApplyAbilityClasses)
	{
		EnemyASC->K2_GiveAbility(AbilityClass);
	}

	// if (EnemyAbilityDataStruct)
	// {
	// 	if (EnemyASC)
	// 	{
	// 		auto GiveAbilityToEnemyASC = [this](const TSoftObjectPtr<UKRGameplayAbility>& InGA)
	// 		{
	// 			UKRGameplayAbility* GA = InGA.LoadSynchronous();
	// 			EnemyASC->GiveAbility(*GA->GetCurrentAbilitySpec());
	// 		};
	//
	// 		GiveAbilityToEnemyASC(EnemyAbilityDataStruct->AttackGA);	
	// 		GiveAbilityToEnemyASC(EnemyAbilityDataStruct->HitGA);	
	// 		GiveAbilityToEnemyASC(EnemyAbilityDataStruct->DeathGA);	
	// 		GiveAbilityToEnemyASC(EnemyAbilityDataStruct->StunGA);	
	// 		GiveAbilityToEnemyASC(EnemyAbilityDataStruct->StunGA);	
	// 		GiveAbilityToEnemyASC(EnemyAbilityDataStruct->SlashGA);	
	// 		GiveAbilityToEnemyASC(EnemyAbilityDataStruct->AlertGA);
	// 	}
	// }

	if (EnemyAttributeDataStruct)
	{
		CombatCommonSet->SetMaxHealth(EnemyAttributeDataStruct->MaxHealth);
		CombatCommonSet->SetAttackPower(EnemyAttributeDataStruct->AttackPower);
		CombatCommonSet->SetDefensePower(EnemyAttributeDataStruct->DefensePower);
		CombatCommonSet->SetDealDamageMult(EnemyAttributeDataStruct->DealDamageMultiply);
		CombatCommonSet->SetTakeDamageMult(EnemyAttributeDataStruct->TakeDamageMultiply);
		CombatCommonSet->SetDamageTaken(EnemyAttributeDataStruct->DamageTaken);

		EnemyAttributeSet->SetCanAttackRange(EnemyAttributeDataStruct->CanAttackRange);
		EnemyAttributeSet->SetEnterRageStatusRate(EnemyAttributeDataStruct->EnterRageStatusRate);
	}
}

bool AKREnemyPawn::TryActivateAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!EnemyASC)
	{
		return false;
	}

	FGameplayAbilitySpec* Spec = EnemyASC->FindAbilitySpecFromClass(AbilityClass);
	if (Spec == nullptr)
	{
		return false;
	}
	
	return EnemyASC->TryActivateAbility(Spec->Handle);
}


