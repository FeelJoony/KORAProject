#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/QuestDataStruct.h"
#include "KRQuestSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestSubSystem, Log, All);

USTRUCT()
struct FQuestDialogueNPCSet
{
	GENERATED_BODY()

	UPROPERTY() TSet<FGameplayTag> NPCTags;
};

UCLASS()
class KORAPROJECT_API UKRQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UKRQuestSubsystem& Get(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = Quest)
	void AcceptQuest(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Quest)
	bool AbandonQuest(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void NotifyObjectiveEvent(FGameplayTag TargetTag, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void NotifyNPCDialogueComplete(FGameplayTag NPCTag);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void MarkNPCDialogueForQuest(FGameplayTag NPCTag);

	UFUNCTION(BlueprintPure, Category = Quest)
	bool HasTalkedToNPCForCurrentQuest(FGameplayTag NPCTag) const;

	UFUNCTION(BlueprintPure, Category = Quest)
	int32 GetCurrentQuestIndex() const;

	UFUNCTION(BlueprintPure, Category = Quest)
	class AKRQuestActor* GetActiveActor() const { return ActiveActor; }
	
	UFUNCTION(BlueprintPure, Category = Quest)
	class UKRQuestInstance* GetActiveInstance() const { return ActiveInstance; }

// Delegate Section
public:
	void InitializeQuestDelegate(class UKRQuestInstance* NewQuestInstance);
	void ReinitializeQuestDelegate(class UKRQuestInstance* NewQuestInstance);
	void UninitializeQuestDelegate();

	void OnAcceptedBroadcast(EQuestType Type, int32 OrderIndex);
	void OnCompletedBroadcast(EQuestType Type, int32 OrderIndex);
	void OnFailedBroadcast(EQuestType Type, int32 OrderIndex);
	void OnSetNextSubBroadcast(EQuestType Type, int32 OrderIndex);
	void OnProgressBroadcast(EQuestType Type, int32 OrderIndex, const struct FSubQuestEvalData& EvalData);
	
// Condition Checker Section
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Quest)
	TMap<FGameplayTag, TSubclassOf<class UQuestConditionChecker>> CheckerGroup;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	TObjectPtr<class AKRQuestActor> ActiveActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	TObjectPtr<class UKRQuestInstance> ActiveInstance;

private:
	UPROPERTY()
	TMap<EQuestType, TObjectPtr<class UQuestDelegate>> QuestDelegateMap;
	
	UPROPERTY() TMap<int32, FQuestDialogueNPCSet> QuestDialogueNPCs;

	TSubclassOf<class AKRQuestActor> LoadQuestActorClass(int32 Index);
};
