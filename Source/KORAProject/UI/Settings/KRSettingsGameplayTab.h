#pragma once

#include "KRSettingsTabBase.h"
#include "KRSettingsGameplayTab.generated.h"

class UComboBoxString;
class UCheckBox;
class USlider;
class UTextBlock;

UCLASS()
class KORAPROJECT_API UKRSettingsGameplayTab : public UKRSettingsTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void RefreshFromSettings(const FKRSettingsData& Settings) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Gameplay")
	TObjectPtr<UComboBoxString> LanguageComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Gameplay")
	TObjectPtr<UCheckBox> ShowFPSCheckBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Gameplay")
	TObjectPtr<USlider> FOVSlider;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Gameplay")
	TObjectPtr<UTextBlock> FOVText;

private:
	void InitializeComboBoxes();
	void BindEvents();

	UFUNCTION() void OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnShowFPSChanged(bool bIsChecked);
	UFUNCTION() void OnFOVChanged(float Value);

	bool bInitializing = false;
};
