#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "KRQuestSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuestSubSystem, Log, All);

UCLASS()
class KORAPROJECT_API UKRQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = Quest)
	void AcceptQuest(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Quest)
	bool AbandonQuest(int32 Index);

	UFUNCTION(BlueprintCallable, Category = Quest)
	void NotifyObjectiveEvent(FGameplayTag TargetTag, int32 Count = 1);

	UFUNCTION(BlueprintPure, Category = Quest)
	class AKRQuestActor* GetActiveActor() const { return ActiveActor; }
	
	UFUNCTION(BlueprintPure, Category = Quest)
	class UKRQuestInstance* GetActiveInstance() const { return ActiveInstance; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	TObjectPtr<class AKRQuestActor> ActiveActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	TObjectPtr<class UKRQuestInstance> ActiveInstance;

private:
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<class UQuestConditionChecker>> Checkers;

	TSubclassOf<class AKRQuestActor> LoadQuestActorClass(int32 Index);
};
