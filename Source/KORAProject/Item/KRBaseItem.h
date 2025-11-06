#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "KRBaseItem.generated.h"

UCLASS(BlueprintType, Blueprintable)
class KORAPROJECT_API UKRBaseItem : public UObject
{
	GENERATED_BODY()

public:
	UKRBaseItem();
	virtual class UWorld* GetWorld() const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FGameplayTagContainer ItemTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 MaxStackSize;

	UPROPERTY(BlueprintReadOnly, Category = "Item")
	int32 Amount;

public:
	UFUNCTION(BlueprintPure, Category = "Item")
	FORCEINLINE FGameplayTagContainer GetItemTags() const { return ItemTags; }

	UFUNCTION(BlueprintPure, Category = "Item")
	FORCEINLINE int32 GetAmount() const { return Amount; }

	UFUNCTION(BlueprintPure, Category = "Item")
	FORCEINLINE int32 GetMaxStackSize() const { return MaxStackSize; }

	UFUNCTION(BlueprintPure, Category = "Item")
	FORCEINLINE FName GetItemID() const { return ItemID; }

	virtual void SetAmount(int32 NewAmount);
};