#include "Settings/KRSettingsSubsystem.h"
#include "Settings/KRSettingsSaveGame.h"
#include "SubSystem/KRSoundSubsystem.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetInternationalizationLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Scalability.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTag/KRSoundTag.h"
#include "Framework/Application/SlateApplication.h"
#include "Internationalization/TextLocalizationManager.h"
#include "Internationalization/Internationalization.h"

void UKRSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	DefaultSettings = FKRSettingsData();
	DefaultSettings.Gameplay.Language = GetLanguageFromCurrentCulture();

	LoadSettings();
	ApplyLanguageImmediate(AppliedSettings.Gameplay.Language);
}

void UKRSettingsSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UKRSettingsSubsystem::SetPendingSettings(const FKRSettingsData& NewSettings)
{
	PendingSettings = NewSettings;
}

void UKRSettingsSubsystem::ApplyPendingSettings(APlayerController* PC)
{
	ApplyVideoSettings(PendingSettings.Video);
	ApplyGraphicsSettings(PendingSettings.Graphics);
	ApplyAudioSettings(PendingSettings.Audio);
	ApplyControlSettings(PendingSettings.Control, PC);
	ApplyGameplaySettings(PendingSettings.Gameplay, PC);

	AppliedSettings = PendingSettings;
	SaveSettings();

	OnSettingsApplied.Broadcast(AppliedSettings);
}

void UKRSettingsSubsystem::RevertPendingSettings()
{
	PendingSettings = AppliedSettings;
}

void UKRSettingsSubsystem::ResetToDefaults()
{
	PendingSettings = DefaultSettings;
}

void UKRSettingsSubsystem::SaveSettings()
{
	UKRSettingsSaveGame* SaveGame = Cast<UKRSettingsSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UKRSettingsSaveGame::StaticClass()));

	if (!SaveGame) return;

	SaveGame->Audio = AppliedSettings.Audio;
	SaveGame->Control = AppliedSettings.Control;
	SaveGame->Gameplay = AppliedSettings.Gameplay;

	UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, SaveUserIndex);
	if (UGameUserSettings* GUS = UGameUserSettings::GetGameUserSettings())
	{
		GUS->SaveSettings();
	}
}

void UKRSettingsSubsystem::LoadSettings()
{
	AppliedSettings = DefaultSettings;
	PendingSettings = DefaultSettings;

	if (UKRSettingsSaveGame* SaveGame = Cast<UKRSettingsSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveUserIndex)))
	{
		AppliedSettings.Audio = SaveGame->Audio;
		AppliedSettings.Control = SaveGame->Control;
		AppliedSettings.Gameplay = SaveGame->Gameplay;
	}
	else
	{
		AppliedSettings.Gameplay.Language = GetLanguageFromCurrentCulture();
	}
	
	PullVideoSettingsFromEngine(AppliedSettings.Video);
	PullGraphicsSettingsFromEngine(AppliedSettings.Graphics);

	PendingSettings = AppliedSettings;
}

void UKRSettingsSubsystem::ApplySettingsOnGameStart(APlayerController* PC)
{
	PendingSettings = AppliedSettings;
	ApplyPendingSettings(PC);
}

TArray<FIntPoint> UKRSettingsSubsystem::GetAvailableResolutions() const
{
	TArray<FIntPoint> Resolutions;
	TArray<FIntPoint> SupportedResolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);

	for (const FIntPoint& Res : SupportedResolutions)
	{
		if (!Resolutions.Contains(Res))
		{
			Resolutions.Add(Res);
		}
	}

	if (Resolutions.Num() == 0)
	{
		Resolutions.Add(FIntPoint(1280, 720));
		Resolutions.Add(FIntPoint(1920, 1080));
		Resolutions.Add(FIntPoint(2560, 1440));
		Resolutions.Add(FIntPoint(3840, 2160));
	}

	Resolutions.Sort([](const FIntPoint& A, const FIntPoint& B)
	{
		return (A.X * A.Y) < (B.X * B.Y);
	});

	return Resolutions;
}

TArray<int32> UKRSettingsSubsystem::GetAvailableFrameRateLimits() const
{
	return { 0, 30, 60, 120, 144, 165, 240 };
}

void UKRSettingsSubsystem::SetPendingResolution(FIntPoint Resolution)
{
	PendingSettings.Video.ScreenResolution = Resolution;
}

void UKRSettingsSubsystem::SetPendingWindowMode(EKRWindowMode Mode)
{
	PendingSettings.Video.WindowMode = Mode;
}

void UKRSettingsSubsystem::SetPendingFrameRateLimit(int32 Limit)
{
	PendingSettings.Video.FrameRateLimit = Limit;
}

void UKRSettingsSubsystem::SetPendingVSync(bool bEnabled)
{
	PendingSettings.Video.bVSync = bEnabled;
}

void UKRSettingsSubsystem::SetPendingOverallQuality(EKRQualityLevel Level)
{
	PendingSettings.Graphics.OverallQuality = Level;
	PendingSettings.Graphics.ViewDistanceQuality = Level;
	PendingSettings.Graphics.AntiAliasingQuality = Level;
	PendingSettings.Graphics.ShadowQuality = Level;
	PendingSettings.Graphics.TextureQuality = Level;
	PendingSettings.Graphics.EffectsQuality = Level;
	PendingSettings.Graphics.PostProcessQuality = Level;
}

void UKRSettingsSubsystem::SetPendingMasterVolume(float Volume)
{
	PendingSettings.Audio.MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UKRSettingsSubsystem::SetPendingMusicVolume(float Volume)
{
	PendingSettings.Audio.MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UKRSettingsSubsystem::SetPendingSFXVolume(float Volume)
{
	PendingSettings.Audio.SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UKRSettingsSubsystem::SetPendingUIVolume(float Volume)
{
	PendingSettings.Audio.UIVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UKRSettingsSubsystem::SetPendingMouseSensitivity(float X, float Y)
{
	PendingSettings.Control.MouseSensitivityX = FMath::Clamp(X, 0.1f, 2.0f);
	PendingSettings.Control.MouseSensitivityY = FMath::Clamp(Y, 0.1f, 2.0f);
}

void UKRSettingsSubsystem::SetPendingLanguage(EKRLanguage Language)
{
	PendingSettings.Gameplay.Language = Language;
}

void UKRSettingsSubsystem::SetPendingShowFPS(bool bShow)
{
	PendingSettings.Gameplay.bShowFPS = bShow;
}

void UKRSettingsSubsystem::SetPendingFOV(float FOV)
{
	PendingSettings.Gameplay.FOV = FMath::Clamp(FOV, 60.0f, 120.0f);
}

void UKRSettingsSubsystem::ApplyVideoSettings(const FKRVideoSettings& Settings)
{
	UGameUserSettings* GUS = UGameUserSettings::GetGameUserSettings();
	if (!GUS) return;
	GUS->SetScreenResolution(Settings.ScreenResolution);
	GUS->SetFullscreenMode(KRSettingsHelpers::ToUEWindowMode(Settings.WindowMode));
	GUS->SetVSyncEnabled(Settings.bVSync);
	GUS->SetFrameRateLimit(Settings.FrameRateLimit <= 0 ? 0.0f : static_cast<float>(Settings.FrameRateLimit));

	const float ClampedScale = FMath::Clamp(Settings.ResolutionScale, 50.0f, 100.0f);
	GUS->SetResolutionScaleValueEx(ClampedScale);

	GUS->SetDynamicResolutionEnabled(Settings.bDynamicResolution);

	GUS->ApplySettings(false);
	GUS->ApplyResolutionSettings(false);
	GUS->ConfirmVideoMode();
}

void UKRSettingsSubsystem::ApplyGraphicsSettings(const FKRGraphicsSettings& Settings)
{
	Scalability::FQualityLevels Quality = Scalability::GetQualityLevels();

	Quality.SetFromSingleQualityLevel(KRSettingsHelpers::ToScalabilityLevel(Settings.OverallQuality));
	Quality.ViewDistanceQuality = KRSettingsHelpers::ToScalabilityLevel(Settings.ViewDistanceQuality);
	Quality.AntiAliasingQuality = KRSettingsHelpers::ToScalabilityLevel(Settings.AntiAliasingQuality);
	Quality.ShadowQuality = KRSettingsHelpers::ToScalabilityLevel(Settings.ShadowQuality);
	Quality.TextureQuality = KRSettingsHelpers::ToScalabilityLevel(Settings.TextureQuality);
	Quality.EffectsQuality = KRSettingsHelpers::ToScalabilityLevel(Settings.EffectsQuality);
	Quality.PostProcessQuality = KRSettingsHelpers::ToScalabilityLevel(Settings.PostProcessQuality);

	Scalability::SetQualityLevels(Quality);

	if (UGameUserSettings* GUS = UGameUserSettings::GetGameUserSettings())
	{
		GUS->ApplySettings(false);
	}
}

void UKRSettingsSubsystem::ApplyAudioSettings(const FKRAudioSettings& Settings)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	UKRSoundSubsystem* SoundSubsystem = World->GetSubsystem<UKRSoundSubsystem>();
	if (!SoundSubsystem) return;

	SoundSubsystem->SetMasterVolume(Settings.MasterVolume);
	SoundSubsystem->SetCategoryVolume(KRTAG_SOUND_SETTINGS_MUSIC, Settings.MusicVolume);
	SoundSubsystem->SetCategoryVolume(KRTAG_SOUND_SETTINGS_SFX, Settings.SFXVolume);
	SoundSubsystem->SetCategoryVolume(KRTAG_SOUND_CATEGORY_UI, Settings.UIVolume);
}

void UKRSettingsSubsystem::ApplyControlSettings(const FKRControlSettings& Settings, APlayerController* PC)
{
	// Mouse sensitivity is applied through KRInputModifier_MouseSensitivity
}

void UKRSettingsSubsystem::ApplyGameplaySettings(const FKRGameplaySettings& Settings, APlayerController* PC)
{
	if (AppliedSettings.Gameplay.Language != Settings.Language)
	{
		const FString Culture = KRSettingsHelpers::GetCultureFromLanguage(Settings.Language);
		FInternationalization::Get().SetCurrentCulture(Culture);
		FTextLocalizationManager::Get().RefreshResources();
		if (FSlateApplication::IsInitialized())
		{
			FSlateApplication::Get().InvalidateAllWidgets(true);
		}
		OnLanguageChanged.Broadcast(Settings.Language);
	}
	
	if (AppliedSettings.Gameplay.bShowFPS != Settings.bShowFPS)
	{
		OnFPSDisplayChanged.Broadcast(Settings.bShowFPS);
	}

	if (!FMath::IsNearlyEqual(AppliedSettings.Gameplay.FOV, Settings.FOV))
	{
		OnFOVChanged.Broadcast(Settings.FOV);
	}

	if (PC && PC->GetPawn())
	{
		TArray<UCameraComponent*> Cameras;
		PC->GetPawn()->GetComponents<UCameraComponent>(Cameras);
		for (UCameraComponent* Camera : Cameras)
		{
			if (Camera)
			{
				Camera->SetFieldOfView(Settings.FOV);
			}
		}
	}
}

void UKRSettingsSubsystem::PullVideoSettingsFromEngine(FKRVideoSettings& OutSettings)
{
	UGameUserSettings* GUS = UGameUserSettings::GetGameUserSettings();
	if (!GUS) return;

	OutSettings.ScreenResolution = GUS->GetScreenResolution();
	OutSettings.WindowMode = KRSettingsHelpers::FromUEWindowMode(GUS->GetFullscreenMode());
	OutSettings.bVSync = GUS->IsVSyncEnabled();
	OutSettings.FrameRateLimit = static_cast<int32>(GUS->GetFrameRateLimit());
	OutSettings.ResolutionScale = GUS->GetResolutionScaleNormalized() * 100.0f;
	OutSettings.bDynamicResolution = GUS->IsDynamicResolutionEnabled();
}

void UKRSettingsSubsystem::PullGraphicsSettingsFromEngine(FKRGraphicsSettings& OutSettings)
{
	const Scalability::FQualityLevels Quality = Scalability::GetQualityLevels();

	OutSettings.OverallQuality = KRSettingsHelpers::FromScalabilityLevel(Quality.GetSingleQualityLevel());
	OutSettings.ViewDistanceQuality = KRSettingsHelpers::FromScalabilityLevel(Quality.ViewDistanceQuality);
	OutSettings.AntiAliasingQuality = KRSettingsHelpers::FromScalabilityLevel(Quality.AntiAliasingQuality);
	OutSettings.ShadowQuality = KRSettingsHelpers::FromScalabilityLevel(Quality.ShadowQuality);
	OutSettings.TextureQuality = KRSettingsHelpers::FromScalabilityLevel(Quality.TextureQuality);
	OutSettings.EffectsQuality = KRSettingsHelpers::FromScalabilityLevel(Quality.EffectsQuality);
	OutSettings.PostProcessQuality = KRSettingsHelpers::FromScalabilityLevel(Quality.PostProcessQuality);
}

void UKRSettingsSubsystem::ApplyLanguageImmediate(EKRLanguage Language)
{
	const FString Culture = KRSettingsHelpers::GetCultureFromLanguage(Language);
	UKismetInternationalizationLibrary::SetCurrentCulture(Culture, true);
	
	FTextLocalizationManager::Get().RefreshResources();
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().InvalidateAllWidgets(true);
	}
}

EKRLanguage UKRSettingsSubsystem::GetLanguageFromCurrentCulture() const
{
	const FString CurrentCulture = UKismetInternationalizationLibrary::GetCurrentCulture();

	if (CurrentCulture.StartsWith(TEXT("ko")))
	{
		return EKRLanguage::Korean;
	}

	return EKRLanguage::English;
}
