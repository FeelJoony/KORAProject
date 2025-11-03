// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "UI/Data/KRItemUIData.h"
#include "KRSlotGridBase.generated.h"

class UCommonButtonBase;
class UUniformGridPanel;
class UKRItemSlotBase;
class UCommonButtonGroupBase;

UCLASS()
class KORAPROJECT_API UKRSlotGridBase : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget)) UUniformGridPanel* SlotGrid = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid") TSubclassOf<UKRItemSlotBase> SlotClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid") int32 Rows = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid") int32 Columns = 4;

	UFUNCTION(BlueprintCallable, Category = "Slots") void InitializeItemGrid(const TArray<FKRItemUIData>& InData);
	UFUNCTION(BlueprintCallable, Category = "Slots") void BuildGrid();

	UFUNCTION(BlueprintCallable, Category = "Slots") int32 GetSelectedIndex() const;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY() UCommonButtonGroupBase* ButtonGroup = nullptr;
	UPROPERTY() TArray<TObjectPtr<UKRItemSlotBase>> Slots;

	void BindSlot(UKRItemSlotBase* CellWidget, int32 CellIndex);
	void ClearGrid();
};
