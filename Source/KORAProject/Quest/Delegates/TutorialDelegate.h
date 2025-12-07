#pragma once

#include "CoreMinimal.h"
#include "QuestDelegate.h"
#include "TutorialDelegate.generated.h"

UCLASS()
class KORAPROJECT_API UTutorialDelegate : public UQuestDelegate
{
	GENERATED_BODY()

public:
	virtual void Initialize(class UKRQuestInstance* NewQuestInstance) override;


protected:
	UFUNCTION(Blueprintable, Category = "Quest|Tutorial")
	void OnAppearTutorialUI(int32 OrderIndex);
};
