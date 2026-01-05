#pragma once

#include "CoreMinimal.h"
#include "QuestDelegate.h"
#include "MainQuestDelegate.generated.h"

UCLASS()
class KORAPROJECT_API UMainQuestDelegate : public UQuestDelegate
{
	GENERATED_BODY()

public:
	virtual void Initialize(class UKRQuestInstance* NewQuestInstance) override;

protected:
	UFUNCTION(Blueprintable, Category = "Quest|Main")
	void OnAppearMainQuestUI(int32 OrderIndex);

	UFUNCTION()
	void OnMainQuestCompleted(int32 OrderIndex);
};
