#include "Slate/MonsterAttributeListWidget.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SMonsterAttributeListWidget::Construct(const FArguments& InArgs)
{
	const FEnemyAttributeDataStruct& AttributeData = InArgs._Attribute.Get();

	DisplayStatValues.Empty();

	DisplayStatValues.Add(MakeShared<FStatValue>(TEXT("MaxHealth"), AttributeData.MaxHealth));
	DisplayStatValues.Add(MakeShared<FStatValue>(TEXT("AttackPower"), AttributeData.AttackPower));
	DisplayStatValues.Add(MakeShared<FStatValue>(TEXT("DefensePower"), AttributeData.DefensePower));
	DisplayStatValues.Add(MakeShared<FStatValue>(TEXT("DealDamageMultiply"), AttributeData.DealDamageMultiply));
	DisplayStatValues.Add(MakeShared<FStatValue>(TEXT("TakeDamageMultiply"), AttributeData.TakeDamageMultiply));
	DisplayStatValues.Add(MakeShared<FStatValue>(TEXT("DamageTaken"), AttributeData.DamageTaken));
	
	ChildSlot
	[
		// Populate the widget
		SNew(SScrollBox)

		+ SScrollBox::Slot()
		[
			ConstructStatListView()
		]
	];
}

TSharedRef<SListView<TSharedPtr<SMonsterAttributeListWidget::FStatValue>>> SMonsterAttributeListWidget::ConstructStatListView()
{
	SAssignNew(ConstructedStatListView, SListView<TSharedPtr<FStatValue>>)
	.ItemHeight(200.f)
	.ListItemsSource(&DisplayStatValues)
	.OnGenerateRow(this, &SMonsterAttributeListWidget::OnGenerateRowForList);

	return ConstructedStatListView.ToSharedRef();
}

TSharedRef<ITableRow> SMonsterAttributeListWidget::OnGenerateRowForList(TSharedPtr<FStatValue> StatValueToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!StatValueToDisplay.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FStatValue>>, OwnerTable);
	}

	static TAttribute<FSlateColor> CachedStatNameTextColor = FSlateColor(FLinearColor::Red);
	static TAttribute<FSlateColor> CachedStatValueTextColor = FSlateColor(FLinearColor::Blue);

	FSlateFontInfo StatNameFont = GetEmbossedTextFont();
	StatNameFont.Size = 15.f;

	FSlateFontInfo StatValueFont = GetEmbossedTextFont();
	StatValueFont.Size = 15.f;

	TSharedRef<STableRow<TSharedPtr<FStatValue>>> ListViewRowWidget
		= SNew(STableRow<TSharedPtr<FStatValue>>, OwnerTable).Padding(FMargin(30.f))
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.FillWidth(0.4f)
				[
					SNew(STextBlock)
					.ColorAndOpacity(CachedStatNameTextColor)
					.Font(StatNameFont)
					.Justification(ETextJustify::Type::Left)
					.Text(FText::FromString(TEXT("StatName: ") + StatValueToDisplay->StatName.ToString()))
				]

				+ SHorizontalBox::Slot()
				.FillWidth(0.2f)
				[
					SNew(SSeparator)
				]

				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.FillWidth(0.4f)
				[
					SNew(STextBlock)
					.ColorAndOpacity(CachedStatValueTextColor)
					.Font(StatValueFont)
					.Justification(ETextJustify::Type::Right)
					.Text(FText::FromString(FString::SanitizeFloat(StatValueToDisplay->StatValue)))
				]
			];

	return ListViewRowWidget;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
