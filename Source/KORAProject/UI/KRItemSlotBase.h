// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommonButtonBase.h"
#include "UI/Data/KRItemUIData.h"
#include "Internationalization/StringTable.h"
#include "UObject/SoftObjectPath.h"
#include "KRItemSlotBase.generated.h"

class UCommonTextBlock;
class UCommonLazyImage;
class UCommonNumericTextBlock;


UENUM(BlueprintType, meta = (Bitflags))
enum class EKRItemField : uint8
{
	None = 0 UMETA(Hidden),
	Icon,
	Name,
	Description,
	Count,
	Price,
	ShopStock,
};
ENUM_CLASS_FLAGS(EKRItemField)

FORCEINLINE int32 Bit(EKRItemField F) { return 1 << static_cast<uint8>(F); }

UCLASS()
class KORAPROJECT_API UKRItemSlotBase : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slot", meta = (Bitmask, BitmaskEnum = "/Script/KORAProject.EKRItemField"))
	int32 VisibleFields = Bit(EKRItemField::Name) | Bit(EKRItemField::Description);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Localization")
	TSoftObjectPtr<UStringTable> ItemStringTableAsset = TSoftObjectPtr<UStringTable>(FSoftObjectPath(TEXT("/Game/UI/StringTable/ST_ItemList.ST_ItemList")));

	UFUNCTION(BlueprintCallable, Category = "Slot") void SetItemData(const FKRItemUIData& InData, int32 OverrideFields = -1);
	UFUNCTION(BlueprintCallable, Category = "Slot") void ClearItemData();
	UFUNCTION(BlueprintCallable) void SetHovered(bool bHovered);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional)) UCommonLazyImage* ItemIcon = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonTextBlock* ItemName = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonTextBlock* ItemDescription = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonNumericTextBlock* ItemCount = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonNumericTextBlock* ItemPrice = nullptr;
	UPROPERTY(meta = (BindWidgetOptional)) UCommonNumericTextBlock* ShopStock = nullptr;

private:
	static FORCEINLINE bool HasField(int32 M, EKRItemField F) { return (M & Bit(F)) != 0; }

	void ApplyFieldsVisibility(int32 FieldsToShow);
	static void SetWidgetVisible(UWidget* W, bool bShow);
	static bool IsDataEmpty(const FKRItemUIData& D);

	void EnsureStringTableLoaded();
	FText GetSourceStringFromTable(const FName& InKey);

	UPROPERTY(Transient)
	TObjectPtr<UStringTable> CachedStringTable;

	bool bStringTableLoaded = false;
};
