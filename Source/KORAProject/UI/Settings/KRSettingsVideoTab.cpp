#include "UI/Settings/KRSettingsVideoTab.h"
#include "Settings/KRSettingsSubsystem.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UKRSettingsVideoTab::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeComboBoxes();
	BindEvents();
}

void UKRSettingsVideoTab::InitializeComboBoxes()
{
	if (!SettingsSubsystem) return;
	if (ResolutionComboBox)
	{
		ResolutionComboBox->ClearOptions();
		AvailableResolutions = SettingsSubsystem->GetAvailableResolutions();

		for (const FIntPoint& Res : AvailableResolutions)
		{
			FString Option = FString::Printf(TEXT("%d x %d"), Res.X, Res.Y);
			ResolutionComboBox->AddOption(Option);
		}
	}

	if (WindowModeComboBox)
	{
		WindowModeComboBox->ClearOptions();
		WindowModeComboBox->AddOption(TEXT("Windowed"));
		WindowModeComboBox->AddOption(TEXT("Fullscreen"));
		WindowModeComboBox->AddOption(TEXT("Borderless"));
	}

	if (FrameRateLimitComboBox)
	{
		FrameRateLimitComboBox->ClearOptions();
		AvailableFrameRates = SettingsSubsystem->GetAvailableFrameRateLimits();

		for (int32 Rate : AvailableFrameRates)
		{
			FString Option = Rate == 0 ? TEXT("Unlimited") : FString::Printf(TEXT("%d FPS"), Rate);
			FrameRateLimitComboBox->AddOption(Option);
		}
	}
}

void UKRSettingsVideoTab::BindEvents()
{
	if (ResolutionComboBox)
	{
		ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsVideoTab::OnResolutionChanged);
	}

	if (WindowModeComboBox)
	{
		WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsVideoTab::OnWindowModeChanged);
	}

	if (FrameRateLimitComboBox)
	{
		FrameRateLimitComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsVideoTab::OnFrameRateLimitChanged);
	}

	if (VSyncCheckBox)
	{
		VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UKRSettingsVideoTab::OnVSyncChanged);
	}

	if (DynamicResolutionCheckBox)
	{
		DynamicResolutionCheckBox->OnCheckStateChanged.AddDynamic(this, &UKRSettingsVideoTab::OnDynamicResolutionChanged);
	}

	if (ResolutionScaleSlider)
	{
		ResolutionScaleSlider->OnValueChanged.AddDynamic(this, &UKRSettingsVideoTab::OnResolutionScaleChanged);
	}

	if (HDRCheckBox)
	{
		HDRCheckBox->OnCheckStateChanged.AddDynamic(this, &UKRSettingsVideoTab::OnHDRChanged);
	}
}

void UKRSettingsVideoTab::RefreshFromSettings(const FKRSettingsData& Settings)
{
	bInitializing = true;

	const FKRVideoSettings& Video = Settings.Video;
	if (ResolutionComboBox)
	{
		for (int32 i = 0; i < AvailableResolutions.Num(); ++i)
		{
			if (AvailableResolutions[i] == Video.ScreenResolution)
			{
				ResolutionComboBox->SetSelectedIndex(i);
				break;
			}
		}
	}

	if (WindowModeComboBox)
	{
		WindowModeComboBox->SetSelectedIndex(static_cast<int32>(Video.WindowMode));
	}

	if (FrameRateLimitComboBox)
	{
		for (int32 i = 0; i < AvailableFrameRates.Num(); ++i)
		{
			if (AvailableFrameRates[i] == Video.FrameRateLimit)
			{
				FrameRateLimitComboBox->SetSelectedIndex(i);
				break;
			}
		}
	}

	if (VSyncCheckBox)
	{
		VSyncCheckBox->SetIsChecked(Video.bVSync);
	}

	if (DynamicResolutionCheckBox)
	{
		DynamicResolutionCheckBox->SetIsChecked(Video.bDynamicResolution);
	}

	if (ResolutionScaleSlider)
	{
		float NormalizedValue = (Video.ResolutionScale - 50.0f) / 50.0f;
		ResolutionScaleSlider->SetValue(NormalizedValue);
	}

	if (ResolutionScaleText)
	{
		ResolutionScaleText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(Video.ResolutionScale))));
	}

	if (HDRCheckBox)
	{
		HDRCheckBox->SetIsChecked(Video.bHDR);
	}

	bInitializing = false;

	Super::RefreshFromSettings(Settings);
}

void UKRSettingsVideoTab::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing || !SettingsSubsystem || !ResolutionComboBox) return;

	int32 Index = ResolutionComboBox->GetSelectedIndex();
	if (AvailableResolutions.IsValidIndex(Index))
	{
		SettingsSubsystem->SetPendingResolution(AvailableResolutions[Index]);
		NotifySettingChanged();
	}
}

void UKRSettingsVideoTab::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing || !SettingsSubsystem || !WindowModeComboBox) return;

	int32 Index = WindowModeComboBox->GetSelectedIndex();
	SettingsSubsystem->SetPendingWindowMode(static_cast<EKRWindowMode>(Index));
	NotifySettingChanged();
}

void UKRSettingsVideoTab::OnFrameRateLimitChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing || !SettingsSubsystem || !FrameRateLimitComboBox) return;

	int32 Index = FrameRateLimitComboBox->GetSelectedIndex();
	if (AvailableFrameRates.IsValidIndex(Index))
	{
		SettingsSubsystem->SetPendingFrameRateLimit(AvailableFrameRates[Index]);
		NotifySettingChanged();
	}
}

void UKRSettingsVideoTab::OnVSyncChanged(bool bIsChecked)
{
	if (bInitializing || !SettingsSubsystem) return;

	SettingsSubsystem->SetPendingVSync(bIsChecked);
	NotifySettingChanged();
}

void UKRSettingsVideoTab::OnDynamicResolutionChanged(bool bIsChecked)
{
	if (bInitializing || !SettingsSubsystem) return;

	FKRSettingsData Pending = SettingsSubsystem->GetPendingSettings();
	Pending.Video.bDynamicResolution = bIsChecked;
	SettingsSubsystem->SetPendingSettings(Pending);
	NotifySettingChanged();
}

void UKRSettingsVideoTab::OnResolutionScaleChanged(float Value)
{
	if (bInitializing || !SettingsSubsystem) return;

	float ScaleValue = 50.0f + (Value * 50.0f);

	if (ResolutionScaleText)
	{
		ResolutionScaleText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(ScaleValue))));
	}

	FKRSettingsData Pending = SettingsSubsystem->GetPendingSettings();
	Pending.Video.ResolutionScale = ScaleValue;
	SettingsSubsystem->SetPendingSettings(Pending);
	NotifySettingChanged();
}

void UKRSettingsVideoTab::OnHDRChanged(bool bIsChecked)
{
	if (bInitializing || !SettingsSubsystem) return;

	FKRSettingsData Pending = SettingsSubsystem->GetPendingSettings();
	Pending.Video.bHDR = bIsChecked;
	SettingsSubsystem->SetPendingSettings(Pending);
	NotifySettingChanged();
}
