#include "Slate/ConstructMonsterWidget.h"
#include "AI/KREnemyPawn.h"
#include "Slate/MonsterAttributeListWidget.h"
#include "Slate/MonsterBlueprintViewport.h"
#include "SubSystem/KRDataTablesSubsystem.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SConstructMonsterWidget::Construct(const FArguments& InArgs, UBlueprint* InBlueprint)
{
	if (!ViewportWidget.IsValid())
	{
		ViewportWidget = SNew(SMonsterBlueprintViewport);
	}

	if (InBlueprint)
	{
		BlueprintAsset = InBlueprint;
		
		ViewportWidget->SetBlueprint(InBlueprint);

		InBlueprint->OnCompiled().AddSP(
			this,
			&SConstructMonsterWidget::OnBlueprintCompiled);

		InBlueprint->OnChanged().AddSP(
			this,
			&SConstructMonsterWidget::OnBlueprintChanged);
	}

	ComboSourceItems.Empty();

	ComboSourceItems.Add(MakeShared<EInfo>(EInfo::Attribute));

	ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.FillHeight(1.f)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			[
				ViewportWidget.IsValid()
					? ViewportWidget.ToSharedRef()
					: SNullWidget::NullWidget
			]

			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			[
				SAssignNew(DisplayInfoVerticalBox, SVerticalBox)

				+ SVerticalBox::Slot()
				.VAlign(VAlign_Top)
				.HAlign(HAlign_Fill)
				.FillHeight(0.1f)
				[
					ConstructInfoComboBox()
				]
			]
		]
	];
}

TSharedRef<SComboBox<TSharedPtr<SConstructMonsterWidget::EInfo>>> SConstructMonsterWidget::ConstructInfoComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<EInfo>>> ConstructedComboBox
		= SNew(SComboBox<TSharedPtr<EInfo>>)
		.OptionsSource(&ComboSourceItems)
		.OnGenerateWidget(this, &SConstructMonsterWidget::OnGenerateComboBoxContent)
		.OnSelectionChanged(this, &SConstructMonsterWidget::OnComboBoxSelectionChanged)
		[
			SAssignNew(ComboDisplayTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("Select Info Type")))
		];

	return ConstructedComboBox;
}

TSharedRef<SWidget> SConstructMonsterWidget::OnGenerateComboBoxContent(TSharedPtr<EInfo> SourceItem)
{
	static FText AssetText = FText::FromString(TEXT("Asset"));
	static FText AbilityText = FText::FromString(TEXT("Ability"));
	static FText AttributeText = FText::FromString(TEXT("Attribute"));
	
	TSharedRef<STextBlock> ConstructedComboText = SNew(STextBlock).Text(InfoTextMap[*SourceItem]);

	return ConstructedComboText;
}

void SConstructMonsterWidget::OnComboBoxSelectionChanged(TSharedPtr<EInfo> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	ComboDisplayTextBlock->SetText(InfoTextMap[*SelectedOption]);

	FEnemyAttributeDataStruct* AttributeData = nullptr;
	if (BlueprintAsset && BlueprintAsset->GeneratedClass)
	{
		if (AKREnemyPawn* EnemyPawn = BlueprintAsset->GeneratedClass->GetDefaultObject<AKREnemyPawn>())
		{
			FGameplayTag EnemyTag = EnemyPawn->GetEnemyTag();
			if (EnemyTag.IsValid())
			{
				UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(EnemyPawn);
				AttributeData = DataTablesSubsystem.GetData<FEnemyAttributeDataStruct>(EnemyTag);
			}
		}
	}
	
	if (*SelectedOption == EInfo::Attribute)
	{
		if (AttributeData)
		{
			if (DisplayInfoVerticalBox->NumSlots() > 1)
			{
				DisplayInfoVerticalBox->GetSlot(1)
				[
					SNew(SMonsterAttributeListWidget)
					.Attribute(*AttributeData)
				];
			}
			
			DisplayInfoVerticalBox->AddSlot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Fill)
			.FillHeight(0.9f)
			[
				SNew(SMonsterAttributeListWidget)
				.Attribute(*AttributeData)
			];	
		}
	}
}

void SConstructMonsterWidget::OnBlueprintCompiled(UBlueprint* Blueprint)
{
	if (ViewportWidget.IsValid())
	{
		ViewportWidget->SetBlueprint(Blueprint);
	}
}

void SConstructMonsterWidget::OnBlueprintChanged(UBlueprint* Blueprint)
{
	UE_LOG(LogTemp, Log, TEXT("Blueprint changed %s"), *Blueprint->GetName());
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
