#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KRSettingsTypes.h"
#include "KRSettingsSubsystem.generated.h"

class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSettingsApplied, const FKRSettingsData&, Settings);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFPSDisplayChanged, bool, bShow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFOVChanged, float, NewFOV);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLanguageChanged, EKRLanguage, NewLanguage);

UCLASS()
class KORAPROJECT_API UKRSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	const FKRSettingsData& GetPendingSettings() const { return PendingSettings; }

	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	const FKRSettingsData& GetAppliedSettings() const { return AppliedSettings; }

	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	void SetPendingSettings(const FKRSettingsData& NewSettings);
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	bool HasPendingChanges() const { return PendingSettings != AppliedSettings; }
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	void ApplyPendingSettings(APlayerController* PC = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	void RevertPendingSettings();
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	void ResetToDefaults();
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	void SaveSettings();
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	void LoadSettings();
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	void ApplySettingsOnGameStart(APlayerController* PC);
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	TArray<FIntPoint> GetAvailableResolutions() const;
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings")
	TArray<int32> GetAvailableFrameRateLimits() const;
	
	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Video")
	void SetPendingResolution(FIntPoint Resolution);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Video")
	void SetPendingWindowMode(EKRWindowMode Mode);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Video")
	void SetPendingFrameRateLimit(int32 Limit);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Video")
	void SetPendingVSync(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Graphics")
	void SetPendingOverallQuality(EKRQualityLevel Level);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Audio")
	void SetPendingMasterVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Audio")
	void SetPendingMusicVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Audio")
	void SetPendingSFXVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Audio")
	void SetPendingUIVolume(float Volume);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Control")
	void SetPendingMouseSensitivity(float X, float Y);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Gameplay")
	void SetPendingLanguage(EKRLanguage Language);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Gameplay")
	void SetPendingShowFPS(bool bShow);

	UFUNCTION(BlueprintCallable, Category = "KR|Settings|Gameplay")
	void SetPendingFOV(float FOV);

	// Events
	UPROPERTY(BlueprintAssignable, Category = "KR|Settings|Events")
	FOnSettingsApplied OnSettingsApplied;

	UPROPERTY(BlueprintAssignable, Category = "KR|Settings|Events")
	FOnFPSDisplayChanged OnFPSDisplayChanged;

	UPROPERTY(BlueprintAssignable, Category = "KR|Settings|Events")
	FOnFOVChanged OnFOVChanged;

	UPROPERTY(BlueprintAssignable, Category = "KR|Settings|Events")
	FOnLanguageChanged OnLanguageChanged;

protected:
	UPROPERTY()
	FKRSettingsData PendingSettings;
	UPROPERTY()
	FKRSettingsData AppliedSettings;
	UPROPERTY()
	FKRSettingsData DefaultSettings;

private:
	void ApplyVideoSettings(const FKRVideoSettings& Settings);
	void ApplyGraphicsSettings(const FKRGraphicsSettings& Settings);
	void ApplyAudioSettings(const FKRAudioSettings& Settings);
	void ApplyControlSettings(const FKRControlSettings& Settings, APlayerController* PC);
	void ApplyGameplaySettings(const FKRGameplaySettings& Settings, APlayerController* PC);

	void PullVideoSettingsFromEngine(FKRVideoSettings& OutSettings);
	void PullGraphicsSettingsFromEngine(FKRGraphicsSettings& OutSettings);

	void ApplyLanguageImmediate(EKRLanguage Language);
	EKRLanguage GetLanguageFromCurrentCulture() const;

	static constexpr const TCHAR* SaveSlotName = TEXT("KRSettings");
	static constexpr int32 SaveUserIndex = 0;
};
