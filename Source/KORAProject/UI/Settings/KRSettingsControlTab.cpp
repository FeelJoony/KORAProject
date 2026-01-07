#include "UI/Settings/KRSettingsControlTab.h"
#include "Settings/KRSettingsSubsystem.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UKRSettingsControlTab::NativeConstruct()
{
	Super::NativeConstruct();
	BindEvents();
}

void UKRSettingsControlTab::BindEvents()
{
	if (MouseSensXSlider)
	{
		MouseSensXSlider->OnValueChanged.AddDynamic(this, &UKRSettingsControlTab::OnMouseSensitivityXChanged);
	}

	if (MouseSensYSlider)
	{
		MouseSensYSlider->OnValueChanged.AddDynamic(this, &UKRSettingsControlTab::OnMouseSensitivityYChanged);
	}
}

void UKRSettingsControlTab::RefreshFromSettings(const FKRSettingsData& Settings)
{
	bInitializing = true;

	const FKRControlSettings& Control = Settings.Control;

	// Mouse Sensitivity X (0.1 ~ 2.0 -> 0.0 ~ 1.0)
	if (MouseSensXSlider)
	{
		float NormalizedValue = (Control.MouseSensitivityX - 0.1f) / 1.9f;
		MouseSensXSlider->SetValue(NormalizedValue);
	}
	UpdateSensitivityText(MouseSensXText, Control.MouseSensitivityX);

	// Mouse Sensitivity Y
	if (MouseSensYSlider)
	{
		float NormalizedValue = (Control.MouseSensitivityY - 0.1f) / 1.9f;
		MouseSensYSlider->SetValue(NormalizedValue);
	}
	UpdateSensitivityText(MouseSensYText, Control.MouseSensitivityY);

	bInitializing = false;

	Super::RefreshFromSettings(Settings);
}

void UKRSettingsControlTab::OnMouseSensitivityXChanged(float Value)
{
	if (bInitializing || !SettingsSubsystem) return;

	// Convert 0.0~1.0 to 0.1~2.0
	float Sensitivity = 0.1f + (Value * 1.9f);
	UpdateSensitivityText(MouseSensXText, Sensitivity);

	FKRSettingsData Pending = SettingsSubsystem->GetPendingSettings();
	Pending.Control.MouseSensitivityX = Sensitivity;
	SettingsSubsystem->SetPendingSettings(Pending);
	NotifySettingChanged();
}

void UKRSettingsControlTab::OnMouseSensitivityYChanged(float Value)
{
	if (bInitializing || !SettingsSubsystem) return;

	float Sensitivity = 0.1f + (Value * 1.9f);
	UpdateSensitivityText(MouseSensYText, Sensitivity);

	FKRSettingsData Pending = SettingsSubsystem->GetPendingSettings();
	Pending.Control.MouseSensitivityY = Sensitivity;
	SettingsSubsystem->SetPendingSettings(Pending);
	NotifySettingChanged();
}

void UKRSettingsControlTab::UpdateSensitivityText(UTextBlock* TextBlock, float Value)
{
	if (TextBlock)
	{
		TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%.2f"), Value)));
	}
}
