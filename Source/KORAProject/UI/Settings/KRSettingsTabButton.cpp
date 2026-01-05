#include "UI/Settings/KRSettingsTabButton.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UKRSettingsTabButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (TabText && !ButtonText.IsEmpty())
	{
		TabText->SetText(ButtonText);
	}

	UpdateVisualState();
}

void UKRSettingsTabButton::SetTabText(const FText& InText)
{
	ButtonText = InText;
	if (TabText)
	{
		TabText->SetText(InText);
	}
}

FText UKRSettingsTabButton::GetTabText() const
{
	return ButtonText;
}

void UKRSettingsTabButton::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();
	UpdateVisualState();
}

void UKRSettingsTabButton::NativeOnSelected(bool bBroadcast)
{
	Super::NativeOnSelected(bBroadcast);
	UpdateVisualState();
}

void UKRSettingsTabButton::NativeOnDeselected(bool bBroadcast)
{
	Super::NativeOnDeselected(bBroadcast);
	UpdateVisualState();
}

void UKRSettingsTabButton::NativeOnHovered()
{
	Super::NativeOnHovered();
	UpdateVisualState();
}

void UKRSettingsTabButton::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	UpdateVisualState();
}

void UKRSettingsTabButton::UpdateVisualState()
{
	if (!TabText) return;

	FLinearColor TextColor = NormalColor;

	if (GetSelected())
	{
		TextColor = SelectedColor;
	}
	else if (IsHovered())
	{
		TextColor = HoveredColor;
	}

	TabText->SetColorAndOpacity(FSlateColor(TextColor));
}
