// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonUserWidget.h"
#include "UI/Data/KRItemUIData.h"
#include "KRSlotGridBase.generated.h"

class UCommonButtonBase;
class UUniformGridPanel;
class UKRItemSlotBase;
class UCommonButtonGroupBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotIndexEvent, int32, Index, UKRItemSlotBase*, Slot);

UCLASS()
class KORAPROJECT_API UKRSlotGridBase : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget)) UUniformGridPanel* SlotGrid = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid") TSubclassOf<UKRItemSlotBase> SlotClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid") int32 Rows = 4;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid") int32 Columns = 4;

	UPROPERTY(BlueprintAssignable, Category = "Slots") FOnSlotIndexEvent OnSelectionChanged;
	UPROPERTY(BlueprintAssignable, Category = "Slots") FOnSlotIndexEvent OnHoverChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slots") bool bSelectOnHover = true; // When Hovered == Selected

	UFUNCTION(BlueprintCallable, Category = "Slots") void InitializeItemGrid(const TArray<FKRItemUIData>& InData);
	UFUNCTION(BlueprintCallable, Category = "Slots") void BuildGrid();

	UFUNCTION(BlueprintCallable, Category = "Slots") int32 GetSelectedIndex() const;
	UFUNCTION(BlueprintPure, Category = "Slots") FORCEINLINE int32 GetNumCells()   const { return Slots.Num(); }
	UFUNCTION(BlueprintPure, Category = "Slots") FORCEINLINE int32 GetRowCount()   const { return Rows; }
	UFUNCTION(BlueprintPure, Category = "Slots") FORCEINLINE int32 GetColumnCount()const { return Columns; }
	UFUNCTION(BlueprintCallable, Category = "Slots") bool SelectIndexSafe(int32 Index);
	UFUNCTION(BlueprintPure, Category = "Slots") UWidget* GetSelectedWidget() const;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY() UCommonButtonGroupBase* ButtonGroup = nullptr;
	UPROPERTY() TArray<TObjectPtr<UKRItemSlotBase>> Slots;

	void BindSlot(UKRItemSlotBase* CellWidget, int32 CellIndex);
	void ClearGrid();

	UFUNCTION() void HandleGroupSelectedChanged(UCommonButtonBase* Selected, int32 SelectedIndex);
	UFUNCTION() void HandleGroupHoveredChanged(UCommonButtonBase* Hovered, int32 HoveredIndex);
};
