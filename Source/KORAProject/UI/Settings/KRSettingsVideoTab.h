#pragma once

#include "KRSettingsTabBase.h"
#include "KRSettingsVideoTab.generated.h"

class UComboBoxString;
class UCheckBox;
class USlider;
class UTextBlock;

UCLASS()
class KORAPROJECT_API UKRSettingsVideoTab : public UKRSettingsTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void RefreshFromSettings(const FKRSettingsData& Settings) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Video")
	TObjectPtr<UComboBoxString> ResolutionComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Video")
	TObjectPtr<UComboBoxString> WindowModeComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Video")
	TObjectPtr<UComboBoxString> FrameRateLimitComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Video")
	TObjectPtr<UCheckBox> VSyncCheckBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Video")
	TObjectPtr<UCheckBox> DynamicResolutionCheckBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Video")
	TObjectPtr<USlider> ResolutionScaleSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Video")
	TObjectPtr<UTextBlock> ResolutionScaleText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Video")
	TObjectPtr<UCheckBox> HDRCheckBox;

private:
	void InitializeComboBoxes();
	void BindEvents();

	UFUNCTION() void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnFrameRateLimitChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnVSyncChanged(bool bIsChecked);
	UFUNCTION() void OnDynamicResolutionChanged(bool bIsChecked);
	UFUNCTION() void OnResolutionScaleChanged(float Value);
	UFUNCTION() void OnHDRChanged(bool bIsChecked);

	TArray<FIntPoint> AvailableResolutions;
	TArray<int32> AvailableFrameRates;

	bool bInitializing = false;
};
