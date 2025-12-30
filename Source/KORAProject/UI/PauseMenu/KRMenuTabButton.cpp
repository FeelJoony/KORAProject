#include "UI/PauseMenu/KRMenuTabButton.h"
#include "Components/Image.h"

void UKRMenuTabButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!TabIcon)
	{
		return;
	}

	UTexture2D* Icon = nullptr;
	if (TObjectPtr<UTexture2D>* Found = TabIconMap.Find(TabType))
	{
		Icon = Found->Get();
	}

	if (!Icon)
	{
		TabIcon->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	TabIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	TabIcon->SetBrushFromTexture(Icon, true);
}
