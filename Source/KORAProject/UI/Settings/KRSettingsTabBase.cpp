#include "UI/Settings/KRSettingsTabBase.h"
#include "Settings/KRSettingsSubsystem.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/Border.h"
#include "Components/PanelWidget.h"
#include "Blueprint/WidgetTree.h"

void UKRSettingsTabBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		SettingsSubsystem = GI->GetSubsystem<UKRSettingsSubsystem>();
	}

	CollectFocusableWidgets();
}

void UKRSettingsTabBase::RefreshFromSettings(const FKRSettingsData& Settings)
{
}

void UKRSettingsTabBase::NotifySettingChanged()
{
	OnSettingChangedDelegate.Broadcast();
}

void UKRSettingsTabBase::CollectFocusableWidgets()
{
	FocusableWidgets.Empty();

	if (WidgetTree)
	{
		WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if (Widget && Widget->IsVisible())
			{
				if (Cast<UComboBoxString>(Widget) ||
					Cast<UCheckBox>(Widget) ||
					Cast<USlider>(Widget))
				{
					FocusableWidgets.Add(Widget);
				}
			}
		});
	}

	FocusableWidgets.Sort([](const UWidget& A, const UWidget& B)
	{
		FGeometry GeoA = A.GetCachedGeometry();
		FGeometry GeoB = B.GetCachedGeometry();

		FVector2D PosA = GeoA.GetAbsolutePosition();
		FVector2D PosB = GeoB.GetAbsolutePosition();

		// Sort by Y position (top to bottom)
		if (!FMath::IsNearlyEqual(PosA.Y, PosB.Y, 10.0f))
		{
			return PosA.Y < PosB.Y;
		}
		return PosA.X < PosB.X;
	});

	CurrentWidgetIndex = 0;
}

void UKRSettingsTabBase::SetFocused(bool bFocused)
{
	bIsFocused = bFocused;

	if (bFocused && FocusableWidgets.Num() == 0)
	{
		CollectFocusableWidgets();
	}

	UpdateFocusVisuals();
}

void UKRSettingsTabBase::HandleNavigatePrev()
{
	if (!bIsFocused || FocusableWidgets.Num() == 0) return;

	UWidget* CurrentWidget = FocusableWidgets.IsValidIndex(CurrentWidgetIndex) ? FocusableWidgets[CurrentWidgetIndex].Get() : nullptr;
	if (!CurrentWidget) return;

	if (UComboBoxString* ComboBox = Cast<UComboBoxString>(CurrentWidget))
	{
		HandleComboBoxPrev(ComboBox);
	}
	else if (USlider* Slider = Cast<USlider>(CurrentWidget))
	{
		HandleSliderPrev(Slider);
	}
}

void UKRSettingsTabBase::HandleNavigateNext()
{
	if (!bIsFocused || FocusableWidgets.Num() == 0) return;

	UWidget* CurrentWidget = FocusableWidgets.IsValidIndex(CurrentWidgetIndex) ? FocusableWidgets[CurrentWidgetIndex].Get() : nullptr;
	if (!CurrentWidget) return;

	if (UComboBoxString* ComboBox = Cast<UComboBoxString>(CurrentWidget))
	{
		HandleComboBoxNext(ComboBox);
	}
	else if (USlider* Slider = Cast<USlider>(CurrentWidget))
	{
		HandleSliderNext(Slider);
	}
}

void UKRSettingsTabBase::HandleNavigateUp()
{
	if (!bIsFocused || FocusableWidgets.Num() == 0) return;

	int32 NewIndex = CurrentWidgetIndex - 1;
	if (NewIndex < 0)
	{
		NewIndex = 0; // Stay at top, don't wrap
	}

	if (NewIndex != CurrentWidgetIndex)
	{
		CurrentWidgetIndex = NewIndex;
		UpdateFocusVisuals();

		UWidget* FocusedWidget = FocusableWidgets.IsValidIndex(CurrentWidgetIndex) ? FocusableWidgets[CurrentWidgetIndex].Get() : nullptr;
	}
}

void UKRSettingsTabBase::HandleNavigateDown()
{
	if (!bIsFocused || FocusableWidgets.Num() == 0) return;

	int32 NewIndex = CurrentWidgetIndex + 1;
	if (NewIndex >= FocusableWidgets.Num())
	{
		NewIndex = FocusableWidgets.Num() - 1;
	}

	if (NewIndex != CurrentWidgetIndex)
	{
		CurrentWidgetIndex = NewIndex;
		UpdateFocusVisuals();

		UWidget* FocusedWidget = FocusableWidgets.IsValidIndex(CurrentWidgetIndex) ? FocusableWidgets[CurrentWidgetIndex].Get() : nullptr;
	}
}

void UKRSettingsTabBase::HandleSelect()
{
	if (!bIsFocused || FocusableWidgets.Num() == 0) return;

	UWidget* CurrentWidget = FocusableWidgets.IsValidIndex(CurrentWidgetIndex) ? FocusableWidgets[CurrentWidgetIndex].Get() : nullptr;
	if (!CurrentWidget) return;

	if (UCheckBox* CheckBox = Cast<UCheckBox>(CurrentWidget))
	{
		HandleCheckBoxSelect(CheckBox);
	}
}

void UKRSettingsTabBase::HandleComboBoxPrev(UComboBoxString* ComboBox)
{
	if (!ComboBox) return;

	int32 CurrentIndex = ComboBox->GetSelectedIndex();
	int32 OptionCount = ComboBox->GetOptionCount();

	if (OptionCount > 0)
	{
		int32 NewIndex = (CurrentIndex - 1 + OptionCount) % OptionCount;
		ComboBox->SetSelectedIndex(NewIndex);
	}
}

void UKRSettingsTabBase::HandleComboBoxNext(UComboBoxString* ComboBox)
{
	if (!ComboBox) return;

	int32 CurrentIndex = ComboBox->GetSelectedIndex();
	int32 OptionCount = ComboBox->GetOptionCount();

	if (OptionCount > 0)
	{
		int32 NewIndex = (CurrentIndex + 1) % OptionCount;
		ComboBox->SetSelectedIndex(NewIndex);
	}
}

void UKRSettingsTabBase::HandleSliderPrev(USlider* Slider)
{
	if (!Slider) return;

	float CurrentValue = Slider->GetValue();
	float NewValue = FMath::Clamp(CurrentValue - SliderStep, 0.0f, 1.0f);
	Slider->SetValue(NewValue);
}

void UKRSettingsTabBase::HandleSliderNext(USlider* Slider)
{
	if (!Slider) return;

	float CurrentValue = Slider->GetValue();
	float NewValue = FMath::Clamp(CurrentValue + SliderStep, 0.0f, 1.0f);
	Slider->SetValue(NewValue);
}

void UKRSettingsTabBase::HandleCheckBoxSelect(UCheckBox* CheckBox)
{
	if (!CheckBox) return;

	CheckBox->SetIsChecked(!CheckBox->IsChecked());
}

void UKRSettingsTabBase::UpdateFocusVisuals()
{
	for (int32 i = 0; i < FocusableWidgets.Num(); ++i)
	{
		if (UWidget* Widget = FocusableWidgets[i].Get())
		{
			bool bWidgetFocused = bIsFocused && (i == CurrentWidgetIndex);
			SetWidgetFocusVisual(Widget, bWidgetFocused);
		}
	}
}

void UKRSettingsTabBase::SetWidgetFocusVisual(UWidget* Widget, bool bFocused)
{
	if (!Widget) return;
	UWidget* Parent = Widget->GetParent();
	while (Parent)
	{
		if (UBorder* Border = Cast<UBorder>(Parent))
		{
			Border->SetBrushColor(bFocused ? FocusedHighlightColor : UnfocusedColor);
			return;
		}
		Parent = Parent->GetParent();
	}
	Widget->SetRenderOpacity(bFocused ? 1.0f : 0.8f);
}
