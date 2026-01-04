#pragma once

#include "KRSettingsTabBase.h"
#include "KRSettingsGraphicsTab.generated.h"

class UComboBoxString;

UCLASS()
class KORAPROJECT_API UKRSettingsGraphicsTab : public UKRSettingsTabBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void RefreshFromSettings(const FKRSettingsData& Settings) override;

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Graphics")
	TObjectPtr<UComboBoxString> OverallQualityComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Graphics")
	TObjectPtr<UComboBoxString> ViewDistanceComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Graphics")
	TObjectPtr<UComboBoxString> AntiAliasingComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Graphics")
	TObjectPtr<UComboBoxString> ShadowQualityComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Graphics")
	TObjectPtr<UComboBoxString> TextureQualityComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Graphics")
	TObjectPtr<UComboBoxString> EffectsQualityComboBox;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Graphics")
	TObjectPtr<UComboBoxString> PostProcessQualityComboBox;

private:
	void InitializeComboBoxes();
	void BindEvents();
	void PopulateQualityComboBox(UComboBoxString* ComboBox);

	UFUNCTION() void OnOverallQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnViewDistanceChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnAntiAliasingChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnShadowQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnTextureQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnEffectsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION() void OnPostProcessQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	void SetQualityLevel(EKRQualityLevel Level, TFunction<void(FKRGraphicsSettings&)> Setter);

	bool bInitializing = false;
};
