#pragma once

#include "CommonActivatableWidget.h"
#include "Settings/KRSettingsTypes.h"
#include "KRSettingsMenuWidget.generated.h"

class UCommonButtonBase;
class UHorizontalBox;
class UOverlay;
class UKRSettingsSubsystem;
class UKRUIRouterSubsystem;
class UKRUIInputSubsystem;
class UKRSettingsTabBase;
class UKRSettingsTabButton;

// Navigation focus row
UENUM(BlueprintType)
enum class EKRSettingsFocusRow : uint8
{
	TabButtons,
	TabContent,
	ActionButtons
};

UENUM(BlueprintType)
enum class EKRSettingsMenuTab : uint8
{
	Video,
	Graphics,
	Control,
	Audio,
	Gameplay
};

UCLASS()
class KORAPROJECT_API UKRSettingsMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UOverlay> TabContentContainer;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UHorizontalBox> TabButtonContainer;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UHorizontalBox> ActionButtonContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Tabs")
	TSubclassOf<UKRSettingsTabBase> VideoTabClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Tabs")
	TSubclassOf<UKRSettingsTabBase> GraphicsTabClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Tabs")
	TSubclassOf<UKRSettingsTabBase> ControlTabClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Tabs")
	TSubclassOf<UKRSettingsTabBase> AudioTabClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Tabs")
	TSubclassOf<UKRSettingsTabBase> GameplayTabClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Tabs")
	TSubclassOf<UKRSettingsTabButton> TabButtonClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|Tabs")
	TArray<FText> TabNames;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Settings|Actions")
	TObjectPtr<UCommonButtonBase> ApplyButton;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Settings|Actions")
	TObjectPtr<UCommonButtonBase> ResetButton;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Settings|Actions")
	TObjectPtr<UCommonButtonBase> BackButton;
	
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SwitchToTab(EKRSettingsMenuTab Tab);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplySettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RevertSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ResetToDefaults();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void CloseMenu();

	UFUNCTION(BlueprintPure, Category = "Settings")
	bool HasPendingChanges() const;

	UFUNCTION(BlueprintPure, Category = "Settings")
	EKRSettingsMenuTab GetCurrentTab() const { return CurrentTab; }

	UFUNCTION(BlueprintPure, Category = "Settings")
	UKRSettingsTabBase* GetCurrentTabWidget() const { return CurrentTabWidget; }

private:
	void SetupTabButtons();
	void BindActionButtons();
	void BindInputActions();
	void UpdateApplyButtonState();

	UFUNCTION()
	void OnLanguageChanged(EKRLanguage NewLanguage);

	UKRSettingsTabBase* CreateTabWidget(EKRSettingsMenuTab Tab);
	void ShowTabWidget(EKRSettingsMenuTab Tab);

	void HandleNavigatePrev();
	void HandleNavigateNext();
	void HandleNavigateUp();
	void HandleNavigateDown();
	void HandleSelect();
	void SetFocusRow(EKRSettingsFocusRow NewRow);
	void UpdateFocusVisuals();

	UFUNCTION()
	void OnTabSettingChanged();

	UPROPERTY()
	TObjectPtr<UKRSettingsSubsystem> SettingsSubsystem;

	UPROPERTY()
	TObjectPtr<UKRUIRouterSubsystem> RouterSubsystem;

	UPROPERTY()
	TObjectPtr<UKRUIInputSubsystem> UIInputSubsystem;

	UPROPERTY()
	TMap<EKRSettingsMenuTab, TObjectPtr<UKRSettingsTabBase>> TabWidgets;

	UPROPERTY()
	TObjectPtr<UKRSettingsTabBase> CurrentTabWidget;

	UPROPERTY()
	TArray<TObjectPtr<UKRSettingsTabButton>> TabButtons;

	UPROPERTY()
	TArray<TObjectPtr<UCommonButtonBase>> ActionButtons;

	EKRSettingsMenuTab CurrentTab = EKRSettingsMenuTab::Video;

	EKRSettingsFocusRow CurrentFocusRow = EKRSettingsFocusRow::TabButtons;
	int32 CurrentFocusIndex = 0;
};
