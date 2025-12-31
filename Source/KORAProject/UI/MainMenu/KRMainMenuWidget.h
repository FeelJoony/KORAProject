#pragma once

#include "CommonActivatableWidget.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRMainMenuWidget.generated.h"

class UCommonButtonBase;
class UKRMenuTabButton;
class UCommonButtonGroupBase;
class UKRUserFacingExperience;

UCLASS()
class KORAPROJECT_API UKRMainMenuWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "MainMenu")
	void BP_OnMenuButtonInvoked(EConfirmContext Context);
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> ContinueButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> NewGameButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> SettingButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> QuitGameButton;

	UPROPERTY()
	TObjectPtr<UCommonButtonGroupBase> ButtonGroup;

	UPROPERTY()
	TArray<TObjectPtr<UCommonButtonBase>> MenuButtons;

	int32 CurrentButtonIndex = 0;

	void InitializeMenuButtons();
	void UpdateMenuSelection(int32 NewIndex);
	
	void HandleMoveUp();
	void HandleMoveDown();
	void HandleSelect();
	
	void BindMenuButton(UCommonButtonBase* Button);
	
	EConfirmContext GetContextForButton(UCommonButtonBase* Button) const;
};
