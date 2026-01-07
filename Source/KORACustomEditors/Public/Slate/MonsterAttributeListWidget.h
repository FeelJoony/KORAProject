#pragma once

#include "CoreMinimal.h"
#include "Data/EnemyAttributeDataStruct.h"
#include "Widgets/SCompoundWidget.h"

class KORACUSTOMEDITORS_API SMonsterAttributeListWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMonsterAttributeListWidget)
		{
		}

	SLATE_ATTRIBUTE(FEnemyAttributeDataStruct, Attribute);	

	SLATE_END_ARGS()

	struct FStatValue
	{
		FName StatName;
		float StatValue;

		FStatValue(FName InName, float InStatValue) : StatName(InName), StatValue(InStatValue)
		{
			
		}
	};

	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<SListView<TSharedPtr<FStatValue>>> ConstructedStatListView;
	TArray<TSharedPtr<FStatValue>> DisplayStatValues;
	
	TSharedRef<SListView<TSharedPtr<FStatValue>>> ConstructStatListView();
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FStatValue> StatValueToDisplay, const TSharedRef<STableViewBase>& OwnerTable);

	FSlateFontInfo GetEmbossedTextFont() const { return FCoreStyle::Get().GetFontStyle("EmbossedText"); }
};
