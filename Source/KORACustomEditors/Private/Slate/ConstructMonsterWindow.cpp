#include "Slate/ConstructMonsterWindow.h"
#include "PropertyCustomizationHelpers.h"
#include "AI/KREnemyPawn.h"
#include "Slate/ConstructMonsterWidget.h"
#include "SubSystem/KRDataTablesSubsystem.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SConstructMonsterWindow::Construct(const FArguments& InArgs)
{
	UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(GEngine->GetWorld());
	DataTablesSubsystem.InitializeDataTables();
	
	ChildSlot
	[
		// Populate the widget

		SAssignNew(BaseVerticalBoxPanel, SVerticalBox)

		+ SVerticalBox::Slot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Center)
		.FillHeight(0.1f)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			.HAlign(HAlign_Right)
			.Padding(FMargin(5.f))
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Class Name : ")))
			]

			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			.HAlign(HAlign_Left)
			.Padding(FMargin(5.f))
			[
				//ConstructMonsterClassPropertyEntryContainer()

				SNew(SClassPropertyEntryBox)
				.MetaClass(AKREnemyPawn::StaticClass())
				.SelectedClass_Lambda([&](){ return SelectedMonsterClass; })
				.OnSetClass(this, &SConstructMonsterWindow::OnSetMonsterClass)
			]
		]
	];
	
}

TSharedRef<SBox> SConstructMonsterWindow::ConstructMonsterClassPropertyEntryContainer()
{
	SAssignNew(ClassPickerContainer, SBox);
	ClassPickerContainer->SetContent
	(
		SNew(SClassPropertyEntryBox)
		.MetaClass(AKREnemyPawn::StaticClass())
		.SelectedClass(TSubclassOf<UClass>())
		.OnSetClass(this, &SConstructMonsterWindow::OnSetMonsterClass)
	);

	return ClassPickerContainer.ToSharedRef();
}

void SConstructMonsterWindow::OnTagChanged(FGameplayTag InTag)
{
	CurrentTag = InTag;
}

void SConstructMonsterWindow::OnSetMonsterClass(const UClass* InClass)
{
	if (const UBlueprintGeneratedClass* BlueprintGeneratedClass = Cast<const UBlueprintGeneratedClass>(InClass))
	{
		if (UBlueprint* Blueprint = Cast<UBlueprint>(BlueprintGeneratedClass->ClassGeneratedBy))
		{
			SelectedMonsterClass = Blueprint->GeneratedClass;
			
			if (BaseVerticalBoxPanel->NumSlots() > 1)
			{
				BaseVerticalBoxPanel->GetSlot(1)
				[
					SNew(SConstructMonsterWidget, Blueprint)
				];
			}
			else
			{
				BaseVerticalBoxPanel->AddSlot()
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.FillHeight(0.9f)
				[
					SNew(SConstructMonsterWidget, Blueprint)	
				];	
			}
		}
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
