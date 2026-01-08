#include "KRAIEnemyController.h"

#include "SubSystem/KRDataTablesSubsystem.h"
#include "KREnemyPawn.h"
#include "Components/StateTreeAIComponent.h"
#include "Data/EnemyDataStruct.h"
#include "Perception/AIPerceptionComponent.h"


AKRAIEnemyController::AKRAIEnemyController()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeAIComponent>(TEXT("StateTreeComponent"));
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
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
			}	
		}
	}
}

