#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Settings/KRSettingsTypes.h"
#include "KRSettingsTabBase.generated.h"

class UKRSettingsSubsystem;
class UComboBoxString;
class UCheckBox;
class USlider;
class UBorder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTabSettingChanged);

UCLASS(Abstract)
class KORAPROJECT_API UKRSettingsTabBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	virtual void RefreshFromSettings(const FKRSettingsData& Settings);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void NotifySettingChanged();

	FOnTabSettingChanged& OnSettingChanged() { return OnSettingChangedDelegate; }

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void HandleNavigatePrev();

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void HandleNavigateNext();

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void HandleNavigateUp();

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void HandleNavigateDown();

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void HandleSelect();

	UFUNCTION(BlueprintCallable, Category = "Navigation")
	void SetFocused(bool bFocused);

	UFUNCTION(BlueprintPure, Category = "Navigation")
	bool IsFocused() const { return bIsFocused; }

	UFUNCTION(BlueprintPure, Category = "Navigation")
	int32 GetFocusedWidgetIndex() const { return CurrentWidgetIndex; }

	UFUNCTION(BlueprintPure, Category = "Navigation")
	int32 GetFocusableWidgetCount() const { return FocusableWidgets.Num(); }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UKRSettingsSubsystem> SettingsSubsystem;
	
	UPROPERTY(EditDefaultsOnly, Category = "Navigation|Style")
	FLinearColor FocusedHighlightColor = FLinearColor(1.0f, 0.8f, 0.2f, 0.3f);

	UPROPERTY(EditDefaultsOnly, Category = "Navigation|Style")
	FLinearColor UnfocusedColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Navigation|Input")
	float SliderStep = 0.05f;

private:
	void CollectFocusableWidgets();
	void UpdateFocusVisuals();
	void SetWidgetFocusVisual(UWidget* Widget, bool bFocused);

	void HandleComboBoxPrev(UComboBoxString* ComboBox);
	void HandleComboBoxNext(UComboBoxString* ComboBox);
	void HandleSliderPrev(USlider* Slider);
	void HandleSliderNext(USlider* Slider);
	void HandleCheckBoxSelect(UCheckBox* CheckBox);
	
	UPROPERTY()
	TArray<TObjectPtr<UWidget>> FocusableWidgets;

	int32 CurrentWidgetIndex = 0;
	bool bIsFocused = false;

	FOnTabSettingChanged OnSettingChangedDelegate;
};
