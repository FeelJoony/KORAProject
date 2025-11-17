// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonActivatableWidget.h"
#include "KRConfirmModal.generated.h"

class UCommonTextBlock;
class UCommonButtonBase;

UCLASS()
class KORAPROJECT_API UKRConfirmModal : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) void SetupConfirm(const FText& InMessage);
	FORCEINLINE void SetOnConfirmed(FSimpleDelegate InOnConfirmed) { OnConfirmed = MoveTemp(InOnConfirmed); };

protected:
	UPROPERTY(meta = (BindWidget)) UCommonTextBlock* AlertText;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* YesButton;
	UPROPERTY(meta = (BindWidget)) UCommonButtonBase* NoButton;

	virtual void NativeOnInitialized() override;
	virtual void NativeOnDeactivated() override;

private:
	FSimpleDelegate OnConfirmed;
	
	UFUNCTION() void HandleYes();
	UFUNCTION() void HandleNo();
};
