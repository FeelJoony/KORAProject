// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "KRQuickSlotWidget.generated.h"

class UKRQuickSlotButtonBase;

UCLASS()
class KORAPROJECT_API UKRQuickSlotWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	void OnQuickSlotMessageReceived(FGameplayTag Channel, const FKRUIMessage_QuickSlot& Message);
	UKRQuickSlotButtonBase* GetSlotButton(FGameplayTag Direction) const;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "QuickSlot") TObjectPtr<UKRQuickSlotButtonBase> NorthQuickSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "QuickSlot") TObjectPtr<UKRQuickSlotButtonBase> EastQuickSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "QuickSlot") TObjectPtr<UKRQuickSlotButtonBase> SouthQuickSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "QuickSlot") TObjectPtr<UKRQuickSlotButtonBase> WestQuickSlot;

	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void UpdateSlot(FGameplayTag Direction, int32 Quantity, const FSoftObjectPath& IconPath);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void ClearSlot(FGameplayTag Direction);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void HighlightSlot(FGameplayTag Direction);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void UpdateSlotQuantity(FGameplayTag Direction, int32 NewQuantity);


	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Item Used From Slot"))
	void BP_OnItemUsedFromSlot(FGameplayTag Direction, int32 Duration);
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Slot Registered"))
	void BP_OnSlotRegistered(FGameplayTag Direction);
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Slot Selection Changed"))
	void BP_OnSlotSelectionChanged(FGameplayTag NewSelectedSlot);

private:
	FGameplayMessageListenerHandle QuickSlotListener;
	FGameplayTag CurrentSelectedSlot = FKRUIMessageTags::QuickSlot_North();
	TMap<FGameplayTag, FName> SlotItemMap;
};
