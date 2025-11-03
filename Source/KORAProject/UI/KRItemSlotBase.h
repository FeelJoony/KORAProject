// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonButtonBase.h"
#include "UI/Data/KRItemUIData.h"
#include "KRItemSlotBase.generated.h"

class UCommonTextBlock;
class UCommonLazyImage;
class UCommonNumericTextBlock;


UENUM(BlueprintType, meta = (Bitflags))
enum class EKRItemField : uint8
{
	None = 0      UMETA(Hidden),
	Icon = 1 << 0,
	Name = 1 << 1,
	Description = 1 << 2,
	Count = 1 << 3,
	Price = 1 << 4,
};
ENUM_CLASS_FLAGS(EKRItemField)


UCLASS()
class KORAPROJECT_API UKRItemSlotBase : public UCommonButtonBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot", meta = (Bitmask, BitmaskEnum = "/Script/KORAProject.EKRItemField"))
	int32 VisibleFields = (int32)(EKRItemField::Name) | (int32)(EKRItemField::Description);

	UFUNCTION(BlueprintCallable, Category = "Slot") void SetItemData(const FKRItemUIData& InData, int32 OverrideFields = -1);
	UFUNCTION(BlueprintCallable, Category = "Slot") void ClearItemData();

	UFUNCTION(BlueprintCallable, Category = "Slot") void SetCellIndex(int32 InIndex) { CellIndex = InIndex; }
	UFUNCTION(BlueprintPure, Category = "Slot") FORCEINLINE int32 GetCellIndex() const { return CellIndex; }

protected:
	UPROPERTY(meta = (BindWidgetOptional)) UCommonLazyImage* ItemIcon = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonTextBlock* ItemName = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonTextBlock* ItemDescription = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonNumericTextBlock* ItemCount = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonNumericTextBlock* ItemPrice = nullptr;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Slot", meta = (AllowPrivateAccess = "true"))
	int32 CellIndex = INDEX_NONE;

	static FORCEINLINE bool HasField(const int32 Mask, const EKRItemField Field) { return (Mask & (int32)Field) != 0; }

	void ApplyFieldsVisibility(int32 FieldsToShow);
	static void SetWidgetVisible(UWidget* W, bool bShow);
	static bool IsDataEmpty(const FKRItemUIData& D);
};
