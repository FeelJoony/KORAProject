#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "Data/QuestDataStruct.h"
#include "Data/SubQuestDataStruct.h"
#include "KRQuestInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestInstance, Log, All);

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	NotStarted			UMETA(DisplayName = "Not Started"),
	InProgress			UMETA(DisplayName = "In Progress"),
	Completed			UMETA(DisplayName = "Completed"),
	Failed				UMETA(DisplayName = "Failed")
};

UCLASS(BlueprintType)
class KORAPROJECT_API UKRQuestInstance : public UObject
{
	GENERATED_BODY()

public:
	UKRQuestInstance();

	void Initialize(int32 QuestIndex, class APlayerState* InOwningPlayer);

	void SetStateTreeAsset(FPrimaryAssetId PrimaryAssetId);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void StartQuest();

	void TickQuest(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void CompleteQuest();

	UFUNCTION(BlueprintCallable, Category = Quest)
	void FailQuest();
	
	UFUNCTION(BlueprintPure, Category = Quest)
	EQuestState GetQuestState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = Quest)
	int32 GetQuestIndex() const { return QuestData.GetKey(); }

	UFUNCTION(BlueprintPure, Category = Quest)
	const FQuestDataStruct& GetQuestData() const { return QuestData; }

	UFUNCTION(BlueprintPure, Category = Quest)
	const FSubQuestDataStruct& GetSubQuestData() const { return SubQuestData; }

	UFUNCTION(BlueprintPure, Category = Quest)
	bool IsQuestCompleted() const { return CurrentState == EQuestState::Completed; }

	UFUNCTION(BlueprintPure, Category = Quest)
	FSubQuestEvalDataStruct GetSubQuestEvalData() const { return SubQuestData.EvalDatas[CurrentSubOrder - 1]; }

	UFUNCTION(BlueprintPure, Category = Quest)
	class UStateTreeComponent* GetStateTreeComponent() const { return StateTreeComponent; }

protected:

	UPROPERTY(BlueprintReadOnly, Category = Quest)
	FQuestDataStruct QuestData;
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	FSubQuestDataStruct SubQuestData;
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	TObjectPtr<class UStateTree> StateTreeAsset;
	UPROPERTY()
	TObjectPtr<class UStateTreeComponent> StateTreeComponent;
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	TObjectPtr<class APlayerState> OwningPlayer;
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	int32 CurrentSubOrder;
	UPROPERTY(BlueprintReadOnly, Category = Quest)
	EQuestState CurrentState;
};
