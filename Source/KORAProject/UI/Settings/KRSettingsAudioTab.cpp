#include "UI/Settings/KRSettingsAudioTab.h"
#include "Settings/KRSettingsSubsystem.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UKRSettingsAudioTab::NativeConstruct()
{
	Super::NativeConstruct();

	BindEvents();
}

void UKRSettingsAudioTab::BindEvents()
{
	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->OnValueChanged.AddDynamic(this, &UKRSettingsAudioTab::OnMasterVolumeChanged);
	}

	if (MusicVolumeSlider)
	{
		MusicVolumeSlider->OnValueChanged.AddDynamic(this, &UKRSettingsAudioTab::OnMusicVolumeChanged);
	}

	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->OnValueChanged.AddDynamic(this, &UKRSettingsAudioTab::OnSFXVolumeChanged);
	}

	if (UIVolumeSlider)
	{
		UIVolumeSlider->OnValueChanged.AddDynamic(this, &UKRSettingsAudioTab::OnUIVolumeChanged);
	}
}

void UKRSettingsAudioTab::RefreshFromSettings(const FKRSettingsData& Settings)
{
	bInitializing = true;

	const FKRAudioSettings& Audio = Settings.Audio;

	// Master Volume
	if (MasterVolumeSlider)
	{
		MasterVolumeSlider->SetValue(Audio.MasterVolume);
	}
	UpdateVolumeText(MasterVolumeText, Audio.MasterVolume);

	// Music Volume
	if (MusicVolumeSlider)
	{
		MusicVolumeSlider->SetValue(Audio.MusicVolume);
	}
	UpdateVolumeText(MusicVolumeText, Audio.MusicVolume);

	// SFX Volume
	if (SFXVolumeSlider)
	{
		SFXVolumeSlider->SetValue(Audio.SFXVolume);
	}
	UpdateVolumeText(SFXVolumeText, Audio.SFXVolume);

	// UI Volume
	if (UIVolumeSlider)
	{
		UIVolumeSlider->SetValue(Audio.UIVolume);
	}
	UpdateVolumeText(UIVolumeText, Audio.UIVolume);

	bInitializing = false;

	Super::RefreshFromSettings(Settings);
}

void UKRSettingsAudioTab::UpdateVolumeText(UTextBlock* TextBlock, float Value)
{
	if (TextBlock)
	{
		int32 Percentage = FMath::RoundToInt(Value * 100.0f);
		TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), Percentage)));
	}
}

void UKRSettingsAudioTab::OnMasterVolumeChanged(float Value)
{
	if (bInitializing || !SettingsSubsystem) return;

	UpdateVolumeText(MasterVolumeText, Value);
	SettingsSubsystem->SetPendingMasterVolume(Value);
	NotifySettingChanged();
}

void UKRSettingsAudioTab::OnMusicVolumeChanged(float Value)
{
	if (bInitializing || !SettingsSubsystem) return;

	UpdateVolumeText(MusicVolumeText, Value);
	SettingsSubsystem->SetPendingMusicVolume(Value);
	NotifySettingChanged();
}

void UKRSettingsAudioTab::OnSFXVolumeChanged(float Value)
{
	if (bInitializing || !SettingsSubsystem) return;

	UpdateVolumeText(SFXVolumeText, Value);
	SettingsSubsystem->SetPendingSFXVolume(Value);
	NotifySettingChanged();
}

void UKRSettingsAudioTab::OnUIVolumeChanged(float Value)
{
	if (bInitializing || !SettingsSubsystem) return;

	UpdateVolumeText(UIVolumeText, Value);
	SettingsSubsystem->SetPendingUIVolume(Value);
	NotifySettingChanged();
}
