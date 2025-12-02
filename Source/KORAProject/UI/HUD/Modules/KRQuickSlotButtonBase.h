// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonButtonBase.h"
#include "GameplayTagContainer.h"
#include "KRQuickSlotButtonBase.generated.h"


UCLASS()
class KORAPROJECT_API UKRQuickSlotButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickSlot") FGameplayTag SlotDirection;
    virtual void NativeOnHovered() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
    void BP_SetSlotData(int32 Quantity, const TSoftObjectPtr<UTexture2D>& Icon);

    UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
    void BP_UpdateQuantity(int32 InQuantity);

    UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
    void BP_ClearSlot();

    UFUNCTION(BlueprintImplementableEvent, Category = "QuickSlot")
    void BP_SetHighlight(float InOpacity);
};
