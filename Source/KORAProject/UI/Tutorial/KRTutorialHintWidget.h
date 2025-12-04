// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "Data/TutorialDataStruct.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRTutorialHintWidget.generated.h"

class UCommonRichTextBlock;

UCLASS()
class KORAPROJECT_API UKRTutorialHintWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeOnInitialized() override;
    virtual void NativeDestruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

protected:
    UPROPERTY(meta = (BindWidget)) class UCommonTextBlock* TutorialText;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial") UDataTable* TutorialPopupTable;

    FTutorialDataStruct CurrentRow;
    bool bHasActiveRow = false;

    FTimerHandle DurationHandle;
    bool bPausedByMe = false;

    FGameplayMessageListenerHandle TutorialListener;

    UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial")
    void BP_OnTutorialShown();

    void HandleTutorialMessage(FGameplayTag Channel, const FKRUIMessage_Tutorial& Msg);
    void ApplyDataTableRow(const FTutorialDataStruct& Row);
    void ClosePopup();

    void OnDurationTimeout();

    bool IsTimedPopup(const FTutorialDataStruct& Row) const;
    bool IsInputPopup(const FTutorialDataStruct& Row) const;
    void ClearTimerAndUnpause();

    void HandleOk();
};
