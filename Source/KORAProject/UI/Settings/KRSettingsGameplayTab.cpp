#include "UI/Settings/KRSettingsGameplayTab.h"
#include "Settings/KRSettingsSubsystem.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UKRSettingsGameplayTab::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeComboBoxes();
	BindEvents();
}

void UKRSettingsGameplayTab::InitializeComboBoxes()
{
	if (LanguageComboBox)
	{
		LanguageComboBox->ClearOptions();
		LanguageComboBox->AddOption(TEXT("한국어"));
		LanguageComboBox->AddOption(TEXT("English"));
	}
}

void UKRSettingsGameplayTab::BindEvents()
{
	if (LanguageComboBox)
	{
		LanguageComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsGameplayTab::OnLanguageChanged);
	}

	if (ShowFPSCheckBox)
	{
		ShowFPSCheckBox->OnCheckStateChanged.AddDynamic(this, &UKRSettingsGameplayTab::OnShowFPSChanged);
	}

	if (FOVSlider)
	{
		FOVSlider->OnValueChanged.AddDynamic(this, &UKRSettingsGameplayTab::OnFOVChanged);
	}
}

void UKRSettingsGameplayTab::RefreshFromSettings(const FKRSettingsData& Settings)
{
	bInitializing = true;

	const FKRGameplaySettings& Gameplay = Settings.Gameplay;

	// Language
	if (LanguageComboBox)
	{
		LanguageComboBox->SetSelectedIndex(static_cast<int32>(Gameplay.Language));
	}

	// Show FPS
	if (ShowFPSCheckBox)
	{
		ShowFPSCheckBox->SetIsChecked(Gameplay.bShowFPS);
	}

	// FOV (60-120 mapped to 0-1)
	if (FOVSlider)
	{
		float NormalizedValue = (Gameplay.FOV - 60.0f) / 60.0f;
		FOVSlider->SetValue(NormalizedValue);
	}

	if (FOVText)
	{
		FOVText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(Gameplay.FOV))));
	}

	bInitializing = false;

	Super::RefreshFromSettings(Settings);
}

void UKRSettingsGameplayTab::OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing || !SettingsSubsystem || !LanguageComboBox) return;

	int32 Index = LanguageComboBox->GetSelectedIndex();
	EKRLanguage Language = static_cast<EKRLanguage>(Index);
	SettingsSubsystem->SetPendingLanguage(Language);
	NotifySettingChanged();
}

void UKRSettingsGameplayTab::OnShowFPSChanged(bool bIsChecked)
{
	if (bInitializing || !SettingsSubsystem) return;

	SettingsSubsystem->SetPendingShowFPS(bIsChecked);
	NotifySettingChanged();
}

void UKRSettingsGameplayTab::OnFOVChanged(float Value)
{
	if (bInitializing || !SettingsSubsystem) return;

	float FOVValue = 60.0f + (Value * 60.0f);

	if (FOVText)
	{
		FOVText->SetText(FText::FromString(FString::Printf(TEXT("%d"), FMath::RoundToInt(FOVValue))));
	}

	SettingsSubsystem->SetPendingFOV(FOVValue);
	NotifySettingChanged();
}
