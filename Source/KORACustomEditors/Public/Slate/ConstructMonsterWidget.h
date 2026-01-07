#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "BlueprintEditor.h"

class KORACUSTOMEDITORS_API SConstructMonsterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SConstructMonsterWidget)
		{
		}

	//SLATE_ARGUMENT(TObjectPtr<class AKREnemyCharacter>, MonsterToConstruct)

	SLATE_END_ARGS()

	enum class EInfo
	{
		Asset,
		Ability,
		Attribute
	};

	void Construct(const FArguments& InArgs, UBlueprint* InBlueprint);

private:
	TObjectPtr<UBlueprint> BlueprintAsset;
	
	TWeakPtr<FBlueprintEditor> BlueprintEditorPtr;
	TSharedPtr<class SMonsterBlueprintViewport> ViewportWidget;

	TSharedPtr<EInfo> CurrentInfo;
	TArray<TSharedPtr<EInfo>> ComboSourceItems;
	TSharedPtr<SVerticalBox> DisplayInfoVerticalBox;
	
	TSharedRef<SComboBox<TSharedPtr<EInfo>>> ConstructInfoComboBox();
	TSharedRef<SWidget> OnGenerateComboBoxContent(TSharedPtr<EInfo> SourceItem);
	void OnComboBoxSelectionChanged(TSharedPtr<EInfo> SelectedOption, ESelectInfo::Type InSelectInfo);

	void OnBlueprintCompiled(UBlueprint* Blueprint);
	void OnBlueprintChanged(UBlueprint* Blueprint);

	TSharedPtr<STextBlock> ComboDisplayTextBlock;

	TMap<EInfo, FText> InfoTextMap =
	{
		{EInfo::Asset, FText::FromString(TEXT("Asset"))},
		{EInfo::Ability, FText::FromString(TEXT("Ability"))},
		{EInfo::Attribute, FText::FromString(TEXT("Attribute"))}
	};
};
