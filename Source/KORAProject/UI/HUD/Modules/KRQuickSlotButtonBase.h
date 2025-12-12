// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonButtonBase.h"
#include "GameplayTagContainer.h"
#include "KRQuickSlotButtonBase.generated.h"

class UMaterialInstanceDynamic;
class UImage;

UCLASS()
class KORAPROJECT_API UKRQuickSlotButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot") FGameplayTag SlotDirection;
    virtual void NativeOnHovered() override;
    virtual void NativeOnClicked() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
    void BP_SetSlotData(int32 Quantity, const TSoftObjectPtr<UTexture2D>& Icon);

    UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
    void BP_UpdateQuantity(int32 InQuantity);

    UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
    void BP_ClearSlot();

    UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
    void BP_SetHighlight(float InOpacity);

    UFUNCTION(BlueprintCallable, Category = "QuickSlot|Duration") 
    void PlayItemDurationTimer(float InDuration);

protected:
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "QuickSlot|Duration") TObjectPtr<UImage> DurationTimer;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickSlot|Duration") FName DurationParameterName = TEXT("Percentage");
    UPROPERTY(Transient) TObjectPtr<UMaterialInstanceDynamic> DurationMID;

    float DurationTotalTime = 0.f;
    float DurationElapsedTime = 0.f;
    float DurationUpdateInterval = 0.02f;

    FTimerHandle DurationTimerHandle;

    void UpdateDurationTimer();
};
