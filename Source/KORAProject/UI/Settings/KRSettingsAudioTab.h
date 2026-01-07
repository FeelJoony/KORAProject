#pragma once

#include "KRSettingsTabBase.h"
#include "KRSettingsAudioTab.generated.h"

class USlider;
class UTextBlock;

UCLASS()
class KORAPROJECT_API UKRSettingsAudioTab : public UKRSettingsTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void RefreshFromSettings(const FKRSettingsData& Settings) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USlider> MasterVolumeSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UTextBlock> MasterVolumeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USlider> MusicVolumeSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UTextBlock> MusicVolumeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USlider> SFXVolumeSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UTextBlock> SFXVolumeText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USlider> UIVolumeSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UTextBlock> UIVolumeText;

private:
	void BindEvents();

	UFUNCTION() void OnMasterVolumeChanged(float Value);
	UFUNCTION() void OnMusicVolumeChanged(float Value);
	UFUNCTION() void OnSFXVolumeChanged(float Value);
	UFUNCTION() void OnUIVolumeChanged(float Value);

	void UpdateVolumeText(UTextBlock* TextBlock, float Value);

	bool bInitializing = false;
};
