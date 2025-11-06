#pragma once

#include "CoreMinimal.h"
#include "Item/Object/KRBaseItem.h"
#include "KREquipItem.generated.h"

UCLASS(Blueprintable, BlueprintType)
class KORAPROJECT_API UKREquipItem : public UKRBaseItem
{
	GENERATED_BODY()

public:
	UKREquipItem();

	/** 장착 시도 */
	UFUNCTION(BlueprintCallable, Category = "장비")
	virtual void Equip();

	/** 장착 해제 시도 */
	UFUNCTION(BlueprintCallable, Category = "장비")
	virtual void Unequip();

	/** 현재 장착 여부 */
	UFUNCTION(BlueprintPure, Category = "장비")
	bool IsEquipped() const { return bIsEquipped; }

protected:
	/** 장착 상태 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "장비")
	bool bIsEquipped = false;
};