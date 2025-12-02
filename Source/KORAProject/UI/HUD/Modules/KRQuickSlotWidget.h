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

	void RefreshFromInventory();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot") // true for HUD, false for Quickslot
	bool bListenGameplayMessages = true;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKRQuickSlotHovered, FGameplayTag, SlotDirection);
	UPROPERTY(BlueprintAssignable, Category = "QuickSlot") FKRQuickSlotHovered OnSlotHovered;
	void NotifySlotHovered(FGameplayTag SlotDir);

protected:
	void OnQuickSlotMessageReceived(FGameplayTag Channel, const FKRUIMessage_QuickSlot& Message);

	UFUNCTION(BlueprintPure, Category = "QuickSlot", meta = (BlueprintProtected))
	UKRQuickSlotButtonBase* GetSlotButton(FGameplayTag Direction) const;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "QuickSlot") TObjectPtr<UKRQuickSlotButtonBase> NorthQuickSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "QuickSlot") TObjectPtr<UKRQuickSlotButtonBase> EastQuickSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "QuickSlot") TObjectPtr<UKRQuickSlotButtonBase> SouthQuickSlot;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "QuickSlot") TObjectPtr<UKRQuickSlotButtonBase> WestQuickSlot;

	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void UpdateSlot(FGameplayTag Direction, int32 Quantity, const TSoftObjectPtr<UTexture2D>& Icon);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void ClearSlot(FGameplayTag Direction);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void HighlightSlot(FGameplayTag Direction);
	UFUNCTION(BlueprintCallable, Category = "QuickSlot") void UpdateSlotQuantity(FGameplayTag Direction, int32 NewQuantity);


	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Item Used From Slot"))
	void BP_OnItemUsedFromSlot(FGameplayTag Direction, int32 Duration);
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Slot Registered"))
	void BP_OnSlotRegistered(FGameplayTag Direction);
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Slot UnRegistered"))
	void BP_OnSlotUnregistered(FGameplayTag Direction);
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "On Slot Selection Changed"))
	void BP_OnSlotSelectionChanged(FGameplayTag NewSelectedSlot);
	UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot", meta = (DisplayName = "Reset All Slots"))
	void BP_ResetAllSlotFrames();

private:
	FGameplayMessageListenerHandle QuickSlotListener;
	FGameplayTag CurrentSelectedSlot = FKRUIMessageTags::QuickSlot_North();

	float UnHightlightedSlotOpacity = 0.6f;
	float HighlightedSlotOpacity = 1.0f;
	float QuantityZeroSlotOpacity = 0.3f;
};
