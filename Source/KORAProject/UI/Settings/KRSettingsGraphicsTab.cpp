#include "UI/Settings/KRSettingsGraphicsTab.h"
#include "Settings/KRSettingsSubsystem.h"
#include "Components/ComboBoxString.h"

void UKRSettingsGraphicsTab::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeComboBoxes();
	BindEvents();
}

void UKRSettingsGraphicsTab::InitializeComboBoxes()
{
	PopulateQualityComboBox(OverallQualityComboBox);
	PopulateQualityComboBox(ViewDistanceComboBox);
	PopulateQualityComboBox(AntiAliasingComboBox);
	PopulateQualityComboBox(ShadowQualityComboBox);
	PopulateQualityComboBox(TextureQualityComboBox);
	PopulateQualityComboBox(EffectsQualityComboBox);
	PopulateQualityComboBox(PostProcessQualityComboBox);
}

void UKRSettingsGraphicsTab::PopulateQualityComboBox(UComboBoxString* ComboBox)
{
	if (!ComboBox) return;

	ComboBox->ClearOptions();
	ComboBox->AddOption(TEXT("Low"));
	ComboBox->AddOption(TEXT("Medium"));
	ComboBox->AddOption(TEXT("High"));
	ComboBox->AddOption(TEXT("Epic"));
}

void UKRSettingsGraphicsTab::BindEvents()
{
	if (OverallQualityComboBox)
	{
		OverallQualityComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsGraphicsTab::OnOverallQualityChanged);
	}
	if (ViewDistanceComboBox)
	{
		ViewDistanceComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsGraphicsTab::OnViewDistanceChanged);
	}
	if (AntiAliasingComboBox)
	{
		AntiAliasingComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsGraphicsTab::OnAntiAliasingChanged);
	}
	if (ShadowQualityComboBox)
	{
		ShadowQualityComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsGraphicsTab::OnShadowQualityChanged);
	}
	if (TextureQualityComboBox)
	{
		TextureQualityComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsGraphicsTab::OnTextureQualityChanged);
	}
	if (EffectsQualityComboBox)
	{
		EffectsQualityComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsGraphicsTab::OnEffectsQualityChanged);
	}
	if (PostProcessQualityComboBox)
	{
		PostProcessQualityComboBox->OnSelectionChanged.AddDynamic(this, &UKRSettingsGraphicsTab::OnPostProcessQualityChanged);
	}
}

void UKRSettingsGraphicsTab::RefreshFromSettings(const FKRSettingsData& Settings)
{
	bInitializing = true;

	const FKRGraphicsSettings& Graphics = Settings.Graphics;

	if (OverallQualityComboBox)
	{
		OverallQualityComboBox->SetSelectedIndex(static_cast<int32>(Graphics.OverallQuality));
	}
	if (ViewDistanceComboBox)
	{
		ViewDistanceComboBox->SetSelectedIndex(static_cast<int32>(Graphics.ViewDistanceQuality));
	}
	if (AntiAliasingComboBox)
	{
		AntiAliasingComboBox->SetSelectedIndex(static_cast<int32>(Graphics.AntiAliasingQuality));
	}
	if (ShadowQualityComboBox)
	{
		ShadowQualityComboBox->SetSelectedIndex(static_cast<int32>(Graphics.ShadowQuality));
	}
	if (TextureQualityComboBox)
	{
		TextureQualityComboBox->SetSelectedIndex(static_cast<int32>(Graphics.TextureQuality));
	}
	if (EffectsQualityComboBox)
	{
		EffectsQualityComboBox->SetSelectedIndex(static_cast<int32>(Graphics.EffectsQuality));
	}
	if (PostProcessQualityComboBox)
	{
		PostProcessQualityComboBox->SetSelectedIndex(static_cast<int32>(Graphics.PostProcessQuality));
	}

	bInitializing = false;

	Super::RefreshFromSettings(Settings);
}

void UKRSettingsGraphicsTab::SetQualityLevel(EKRQualityLevel Level, TFunction<void(FKRGraphicsSettings&)> Setter)
{
	if (!SettingsSubsystem) return;

	FKRSettingsData Pending = SettingsSubsystem->GetPendingSettings();
	Setter(Pending.Graphics);
	SettingsSubsystem->SetPendingSettings(Pending);
	NotifySettingChanged();
}

void UKRSettingsGraphicsTab::OnOverallQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing || !SettingsSubsystem) return;

	EKRQualityLevel Level = static_cast<EKRQualityLevel>(OverallQualityComboBox->GetSelectedIndex());
	SettingsSubsystem->SetPendingOverallQuality(Level);

	// Refresh UI to show all settings updated
	RefreshFromSettings(SettingsSubsystem->GetPendingSettings());
	NotifySettingChanged();
}

void UKRSettingsGraphicsTab::OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing) return;
	EKRQualityLevel Level = static_cast<EKRQualityLevel>(ViewDistanceComboBox->GetSelectedIndex());
	SetQualityLevel(Level, [Level](FKRGraphicsSettings& G) { G.ViewDistanceQuality = Level; });
}

void UKRSettingsGraphicsTab::OnAntiAliasingChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing) return;
	EKRQualityLevel Level = static_cast<EKRQualityLevel>(AntiAliasingComboBox->GetSelectedIndex());
	SetQualityLevel(Level, [Level](FKRGraphicsSettings& G) { G.AntiAliasingQuality = Level; });
}

void UKRSettingsGraphicsTab::OnShadowQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing) return;
	EKRQualityLevel Level = static_cast<EKRQualityLevel>(ShadowQualityComboBox->GetSelectedIndex());
	SetQualityLevel(Level, [Level](FKRGraphicsSettings& G) { G.ShadowQuality = Level; });
}

void UKRSettingsGraphicsTab::OnTextureQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing) return;
	EKRQualityLevel Level = static_cast<EKRQualityLevel>(TextureQualityComboBox->GetSelectedIndex());
	SetQualityLevel(Level, [Level](FKRGraphicsSettings& G) { G.TextureQuality = Level; });
}

void UKRSettingsGraphicsTab::OnEffectsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing) return;
	EKRQualityLevel Level = static_cast<EKRQualityLevel>(EffectsQualityComboBox->GetSelectedIndex());
	SetQualityLevel(Level, [Level](FKRGraphicsSettings& G) { G.EffectsQuality = Level; });
}

void UKRSettingsGraphicsTab::OnPostProcessQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bInitializing) return;
	EKRQualityLevel Level = static_cast<EKRQualityLevel>(PostProcessQualityComboBox->GetSelectedIndex());
	SetQualityLevel(Level, [Level](FKRGraphicsSettings& G) { G.PostProcessQuality = Level; });
}
