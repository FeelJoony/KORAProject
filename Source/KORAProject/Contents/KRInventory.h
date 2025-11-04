#pragma once

#include "CoreMinimal.h"
#include "Contents/KRContents.h"
#include "KRInventory.generated.h"

UCLASS()
class KORAPROJECT_API UKRInventory : public UKRContents
{
	GENERATED_BODY()

public:
	UKRInventory();

	virtual void Initialize() override;
	virtual void DeInitialize() override;

	virtual void SetUserData() override;


private:
	TSharedPtr<struct FItemGameplayTagContainer> ItemContainer;
};
