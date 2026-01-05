#include "UI/Settings/KRSettingsActionButton.h"
#include "Components/TextBlock.h"

void UKRSettingsActionButton::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ButtonText.IsEmpty())
	{
		switch (ActionType)
		{
		case EKRSettingsActionType::Apply:
			ButtonText = NSLOCTEXT("Settings", "Apply", "APPLY");
			break;
		case EKRSettingsActionType::Reset:
			ButtonText = NSLOCTEXT("Settings", "Reset", "RESET");
			break;
		case EKRSettingsActionType::Back:
			ButtonText = NSLOCTEXT("Settings", "Back", "BACK");
			break;
		}
	}

	if (ButtonTextBlock)
	{
		ButtonTextBlock->SetText(ButtonText);
	}

	UpdateVisualState();
}

void UKRSettingsActionButton::SetButtonText(const FText& InText)
{
	ButtonText = InText;
	if (ButtonTextBlock)
	{
		ButtonTextBlock->SetText(InText);
	}
}

void UKRSettingsActionButton::SetActionType(EKRSettingsActionType InType)
{
	ActionType = InType;
}

void UKRSettingsActionButton::NativeOnHovered()
{
	Super::NativeOnHovered();
	UpdateVisualState();
}

void UKRSettingsActionButton::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	UpdateVisualState();
}

void UKRSettingsActionButton::UpdateVisualState()
{
	if (!ButtonTextBlock) return;

	FLinearColor TextColor;

	if (!GetIsEnabled())
	{
		TextColor = DisabledColor;
	}
	else if (IsHovered())
	{
		TextColor = HoveredColor;
	}
	else
	{
		TextColor = NormalColor;
	}

	ButtonTextBlock->SetColorAndOpacity(FSlateColor(TextColor));
}
