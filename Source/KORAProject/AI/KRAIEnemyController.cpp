#include "KRAIEnemyController.h"

#include "SubSystem/KRDataTablesSubsystem.h"
#include "KREnemyPawn.h"
#include "Components/StateTreeAIComponent.h"
#include "Data/EnemyDataStruct.h"
#include "GameplayTag/KREnemyTag.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISenseConfig_Damage.h"

AKRAIEnemyController::AKRAIEnemyController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerceptionComponent);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1000.f;
	//SightConfig->LoseSightRadius = 2400.f;
	SightConfig->PeripheralVisionAngleDegrees = 75.f;
	SightConfig->SetMaxAge(1.8f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->ConfigureSense(*DamageConfig);
	
	PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::HandleTargetPerceptionUpdated);
}

void AKRAIEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AKREnemyPawn* EnemyPawn = Cast<AKREnemyPawn>(InPawn))
	{
		FGameplayTag EnemyTag = EnemyPawn->GetEnemyTag();

		const FEnemyDataStruct* EnemyDataStruct = nullptr;

		UWorld* World = GetWorld();
		if (World == nullptr)
		{
			static const FString TablePath = TEXT("/Game/Data/DataTables");

			static const FString EnemyTableName = TEXT("EnemyData");

			UDataTable* DataTable = Cast<class UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *FPaths::Combine(TablePath, EnemyTableName)));

			TArray<FEnemyDataStruct*> DataArray;
			DataTable->GetAllRows(TEXT(""), DataArray);

			for (const FEnemyDataStruct* Data : DataArray)
			{
				if (Data->EnemyTag == EnemyTag)
				{
					EnemyDataStruct = Data;

					break;
				}
			}
		}
		else
		{
			UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(this);
		
			EnemyDataStruct = DataTablesSubsystem.GetData<FEnemyDataStruct>(EnemyTag);
		}

		if (EnemyDataStruct)
		{
			if (UStateTree* StateTree = EnemyDataStruct->StateTree.LoadSynchronous())
			{
				StateTreeComponent->SetStateTree(StateTree);
				StateTreeComponent->StartLogic();
			}	
		}
	}
}

void AKRAIEnemyController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !GetWorld()) return;

	const bool bIsSight = (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>()) && IsPlayerCharacter(Actor);
	const bool bIsDamage = (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>()) && IsPlayerCharacter(Actor);
	
	if (!bIsSight && !bIsDamage) return;
	
	APawn* PlayerPawn0 = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn0)
	{
		return;
	}

	if (TargetActor)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		TargetActor = PlayerPawn0;
		SetFocus(PlayerPawn0);

		if (StateTreeComponent)
		{
			StateTreeComponent->SendStateTreeEvent(KRTAG_ENEMY_AISTATE_COMBAT);
		}
	}
	else
	{
		if (GetFocusActor() == Actor)
		{
			ClearFocus(EAIFocusPriority::Gameplay);
		}
		TargetActor = nullptr;
	}
}

bool AKRAIEnemyController::IsPlayerCharacter(AActor* InActor)
{
	if (!InActor || !GetWorld()) return false;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn) return false;

	APawn* SensedPawn = ResolveToPawn(InActor);
	return (SensedPawn && SensedPawn == PlayerPawn);
}

APawn* AKRAIEnemyController::ResolveToPawn(AActor* InActor) const
{
	if (!InActor) return nullptr;
	if (APawn* AsPawn = Cast<APawn>(InActor)) return AsPawn;
	if (APawn* InstigatorPawn = InActor->GetInstigator()) return InstigatorPawn;

	AActor* OwnerActor = InActor->GetOwner();
	while (OwnerActor)
	{
		if (APawn* OwnerPawn = Cast<APawn>(OwnerActor))
		{
			return OwnerPawn;
		}
		OwnerActor = OwnerActor->GetOwner();
	}
	return nullptr;
}


