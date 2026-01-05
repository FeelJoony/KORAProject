#pragma once

#include "CoreMinimal.h"
#include "KRSettingsTypes.generated.h"

UENUM(BlueprintType)
enum class EKRWindowMode : uint8
{
	Windowed UMETA(DisplayName = "Windowed"),
	Fullscreen UMETA(DisplayName = "Fullscreen"),
	Borderless UMETA(DisplayName = "Borderless")
};

UENUM(BlueprintType)
enum class EKRQualityLevel : uint8
{
	Low = 0 UMETA(DisplayName = "Low"),
	Medium = 1 UMETA(DisplayName = "Medium"),
	High = 2 UMETA(DisplayName = "High"),
	Epic = 3 UMETA(DisplayName = "Epic")
};

UENUM(BlueprintType)
enum class EKRLanguage : uint8
{
	Korean UMETA(DisplayName = "한국어"),
	English UMETA(DisplayName = "English")
};

USTRUCT(BlueprintType)
struct FKRVideoSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video")
	FIntPoint ScreenResolution = FIntPoint(1920, 1080);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video")
	EKRWindowMode WindowMode = EKRWindowMode::Borderless;

	// 0 = Unlimited
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video", meta = (ClampMin = "0", ClampMax = "240"))
	int32 FrameRateLimit = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video")
	bool bVSync = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video")
	bool bDynamicResolution = false;

	// 50~100 (ScreenPercentage)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video", meta = (ClampMin = "50", ClampMax = "100"))
	float ResolutionScale = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Video")
	bool bHDR = false;

	bool operator==(const FKRVideoSettings& Other) const
	{
		return ScreenResolution == Other.ScreenResolution
			&& WindowMode == Other.WindowMode
			&& FrameRateLimit == Other.FrameRateLimit
			&& bVSync == Other.bVSync
			&& bDynamicResolution == Other.bDynamicResolution
			&& FMath::IsNearlyEqual(ResolutionScale, Other.ResolutionScale)
			&& bHDR == Other.bHDR;
	}

	bool operator!=(const FKRVideoSettings& Other) const { return !(*this == Other); }
};

USTRUCT(BlueprintType)
struct FKRGraphicsSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	EKRQualityLevel OverallQuality = EKRQualityLevel::High;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	EKRQualityLevel ViewDistanceQuality = EKRQualityLevel::High;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	EKRQualityLevel AntiAliasingQuality = EKRQualityLevel::High;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	EKRQualityLevel ShadowQuality = EKRQualityLevel::High;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	EKRQualityLevel TextureQuality = EKRQualityLevel::High;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	EKRQualityLevel EffectsQuality = EKRQualityLevel::High;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	EKRQualityLevel PostProcessQuality = EKRQualityLevel::High;

	bool operator==(const FKRGraphicsSettings& Other) const
	{
		return OverallQuality == Other.OverallQuality
			&& ViewDistanceQuality == Other.ViewDistanceQuality
			&& AntiAliasingQuality == Other.AntiAliasingQuality
			&& ShadowQuality == Other.ShadowQuality
			&& TextureQuality == Other.TextureQuality
			&& EffectsQuality == Other.EffectsQuality
			&& PostProcessQuality == Other.PostProcessQuality;
	}

	bool operator!=(const FKRGraphicsSettings& Other) const { return !(*this == Other); }
};

USTRUCT(BlueprintType)
struct FKRControlSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float MouseSensitivityX = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float MouseSensitivityY = 1.0f;

	bool operator==(const FKRControlSettings& Other) const
	{
		return FMath::IsNearlyEqual(MouseSensitivityX, Other.MouseSensitivityX)
			&& FMath::IsNearlyEqual(MouseSensitivityY, Other.MouseSensitivityY);
	}

	bool operator!=(const FKRControlSettings& Other) const { return !(*this == Other); }
};

USTRUCT(BlueprintType)
struct FKRAudioSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MasterVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MusicVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float UIVolume = 1.0f;

	bool operator==(const FKRAudioSettings& Other) const
	{
		return FMath::IsNearlyEqual(MasterVolume, Other.MasterVolume)
			&& FMath::IsNearlyEqual(MusicVolume, Other.MusicVolume)
			&& FMath::IsNearlyEqual(SFXVolume, Other.SFXVolume)
			&& FMath::IsNearlyEqual(UIVolume, Other.UIVolume);
	}

	bool operator!=(const FKRAudioSettings& Other) const { return !(*this == Other); }
};

USTRUCT(BlueprintType)
struct FKRGameplaySettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	EKRLanguage Language = EKRLanguage::English;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	bool bShowFPS = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay", meta = (ClampMin = "60.0", ClampMax = "120.0"))
	float FOV = 90.0f;

	bool operator==(const FKRGameplaySettings& Other) const
	{
		return Language == Other.Language
			&& bShowFPS == Other.bShowFPS
			&& FMath::IsNearlyEqual(FOV, Other.FOV);
	}

	bool operator!=(const FKRGameplaySettings& Other) const { return !(*this == Other); }
};

USTRUCT(BlueprintType)
struct FKRSettingsData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FKRVideoSettings Video;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FKRGraphicsSettings Graphics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FKRControlSettings Control;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FKRAudioSettings Audio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FKRGameplaySettings Gameplay;

	bool operator==(const FKRSettingsData& Other) const
	{
		return Video == Other.Video
			&& Graphics == Other.Graphics
			&& Control == Other.Control
			&& Audio == Other.Audio
			&& Gameplay == Other.Gameplay;
	}

	bool operator!=(const FKRSettingsData& Other) const { return !(*this == Other); }
};

// Helper functions
namespace KRSettingsHelpers
{
	inline FString GetCultureFromLanguage(EKRLanguage Language)
	{
		switch (Language)
		{
		case EKRLanguage::Korean: return TEXT("ko");
		case EKRLanguage::English: return TEXT("en");
		default: return TEXT("ko");
		}
	}

	inline EKRLanguage GetLanguageFromCulture(const FString& Culture)
	{
		if (Culture.StartsWith(TEXT("en"))) return EKRLanguage::English;
		return EKRLanguage::Korean;
	}

	inline EWindowMode::Type ToUEWindowMode(EKRWindowMode Mode)
	{
		switch (Mode)
		{
		case EKRWindowMode::Fullscreen: return EWindowMode::Fullscreen;
		case EKRWindowMode::Borderless: return EWindowMode::WindowedFullscreen;
		default: return EWindowMode::Windowed;
		}
	}

	inline EKRWindowMode FromUEWindowMode(EWindowMode::Type Mode)
	{
		switch (Mode)
		{
		case EWindowMode::Fullscreen: return EKRWindowMode::Fullscreen;
		case EWindowMode::WindowedFullscreen: return EKRWindowMode::Borderless;
		default: return EKRWindowMode::Windowed;
		}
	}

	inline int32 ToScalabilityLevel(EKRQualityLevel Level)
	{
		return FMath::Clamp(static_cast<int32>(Level), 0, 3);
	}

	inline EKRQualityLevel FromScalabilityLevel(int32 Level)
	{
		return static_cast<EKRQualityLevel>(FMath::Clamp(Level, 0, 3));
	}
}
