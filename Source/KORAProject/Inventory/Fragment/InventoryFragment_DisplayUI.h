#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_DisplayUI.generated.h"

UCLASS(BlueprintType, Blueprintable)
class KORAPROJECT_API UInventoryFragment_DisplayUI : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName DisplayNameKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName DescriptionKey;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Price = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 WeaponLevel = 0;

	const UTexture2D* GetIcon() const
	{
		if (ItemIcon.IsPending())
		{
			return ItemIcon.LoadSynchronous();
		}

		return ItemIcon.Get();
	}

	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Ability.Item.DisplayUI"); }

	virtual void OnInstanceCreated(UKRInventoryItemInstance* Instance) override;
};
