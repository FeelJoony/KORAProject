#pragma once

#include "CoreMinimal.h"
#include "Inventory/KRInventoryItemDefinition.h"
#include "InventoryFragment_SetIcon.generated.h"

UCLASS()
class KORAPROJECT_API UInventoryFragment_SetIcon : public UKRInventoryItemFragment
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetIcon(TSoftObjectPtr<UTexture2D> NewIcon)
	{
		IconTexture = NewIcon;
	}

	const UTexture2D* GetIcon() const
	{
		if (IconTexture.IsPending())
		{
			return IconTexture.LoadSynchronous();
		}

		return IconTexture.Get();
	}

	virtual FGameplayTag GetFragmentTag() const override { return FGameplayTag::RequestGameplayTag("Ability.Item.SetIcon"); }
	
private:
	TSoftObjectPtr<UTexture2D> IconTexture;
	
};
