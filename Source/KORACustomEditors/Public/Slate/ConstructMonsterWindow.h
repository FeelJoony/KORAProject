#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Widgets/SCompoundWidget.h"

class KORACUSTOMEDITORS_API SConstructMonsterWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SConstructMonsterWindow)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TSharedPtr<SVerticalBox> BaseVerticalBoxPanel;
private:
	FGameplayTag CurrentTag;

	TSubclassOf<class AKREnemyPawn> SelectedMonsterClass;
	
	TSharedPtr<SBox> ClassPickerContainer;

	TSharedRef<SBox> ConstructMonsterClassPropertyEntryContainer();
	
	void OnTagChanged(FGameplayTag Tag);
	void OnSetMonsterClass(const UClass* InClass);
};
