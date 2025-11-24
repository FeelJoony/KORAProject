// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRQuickSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;

UCLASS()
class KORAPROJECT_API UKRQuickSlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	void OnQuickSlotMessageReceived(FGameplayTag Channel, const FKRUIMessage_QuickSlot& Message);

	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void UpdateSlot(EQuickSlotDirection Direction, int32 Quantity, const FSoftObjectPath& IconPath);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void ClearSlot(EQuickSlotDirection Direction);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void HighlightSlot(EQuickSlotDirection Direction);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void UpdateSlotQuantity(EQuickSlotDirection Direction, int32 NewQuantity);


	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Item Used From Slot"))
	void BP_OnItemUsedFromSlot(EQuickSlotDirection Direction, int32 Duration);
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Slot Registered"))
	void BP_OnSlotRegistered(EQuickSlotDirection Direction);
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Slot Selection Changed"))
	void BP_OnSlotSelectionChanged(EQuickSlotDirection NewSelectedSlot);

	void GetSlotWidgets(EQuickSlotDirection Direction, UImage*& OutIcon, UTextBlock*& OutQuantity, UBorder*& OutBorder);

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> NorthSlotIcon;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> NorthSlotQuantity;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> EastSlotIcon;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> EastSlotQuantity;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> SouthSlotIcon;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> SouthSlotQuantity;

	UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> WestSlotIcon;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> WestSlotQuantity;


	UPROPERTY(EditDefaultsOnly, Category = "QuickSlot|Visual") FLinearColor HighlightColor = FLinearColor::Yellow;
	UPROPERTY(EditDefaultsOnly, Category = "QuickSlot|Visual") FLinearColor NormalColor = FLinearColor::White;
	UPROPERTY(EditDefaultsOnly, Category = "QuickSlot|Visual") float EmptySlotOpacity = 0.3f;

private:
	FGameplayMessageListenerHandle QuickSlotListener;
	EQuickSlotDirection CurrentSelectedSlot = EQuickSlotDirection::North;
	TMap<EQuickSlotDirection, FName> SlotItemMap;
};
