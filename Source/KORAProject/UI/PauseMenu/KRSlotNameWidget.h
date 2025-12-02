// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "KRSlotNameWidget.generated.h"

class UCommonTextBlock;
class UCommonButtonBase;

UENUM(BlueprintType)
enum class EKRSlotNameContext : uint8
{
	Menu,
	QuickSlot_HasItem,
	QuickSlot_Empty
};

UCLASS()
class KORAPROJECT_API UKRSlotNameWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")	FName StringTableId = TEXT("/Game/UI/StringTable/ST_UIBaseTexts");
	UPROPERTY(BlueprintReadOnly, Category = "SlotName")						EKRSlotNameContext Context = EKRSlotNameContext::Menu;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UCommonTextBlock> LabelText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UCommonButtonBase> PrimaryButton;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UCommonButtonBase> SecondaryButton;

	void SetupForMenu(FName MenuNameKey);
	void SetupForQuickSlot(FName ItemNameKey);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKRSlotNamePrimaryClicked, EKRSlotNameContext, Context);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKRSlotNameSecondaryClicked, EKRSlotNameContext, Context);

	UPROPERTY(BlueprintAssignable, Category = "SlotName") FKRSlotNamePrimaryClicked OnPrimaryClicked;
	UPROPERTY(BlueprintAssignable, Category = "SlotName") FKRSlotNameSecondaryClicked OnSecondaryClicked;

	UFUNCTION(BlueprintImplementableEvent, Category = "SlotName")
	void BP_SetButtonText(UCommonButtonBase* Button, const FText& Text);

protected:
	virtual void NativeConstruct() override;
	FText MakeText(FName Key) const;

	UFUNCTION() void HandlePrimaryClicked();
	UFUNCTION() void HandleSecondaryClicked();
};
