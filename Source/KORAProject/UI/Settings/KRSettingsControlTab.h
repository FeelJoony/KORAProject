#pragma once

#include "KRSettingsTabBase.h"
#include "KRSettingsControlTab.generated.h"

class USlider;
class UTextBlock;

UCLASS()
class KORAPROJECT_API UKRSettingsControlTab : public UKRSettingsTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void RefreshFromSettings(const FKRSettingsData& Settings) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Control")
	TObjectPtr<USlider> MouseSensXSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Control")
	TObjectPtr<UTextBlock> MouseSensXText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Control")
	TObjectPtr<USlider> MouseSensYSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Control")
	TObjectPtr<UTextBlock> MouseSensYText;

private:
	void BindEvents();

	UFUNCTION() void OnMouseSensitivityXChanged(float Value);
	UFUNCTION() void OnMouseSensitivityYChanged(float Value);

	void UpdateSensitivityText(UTextBlock* TextBlock, float Value);

	bool bInitializing = false;
};
