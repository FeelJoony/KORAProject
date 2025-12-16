#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "CurrencyDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FCurrencyDataStruct  : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FCurrencyDataStruct ()
		: Index(-1)
		, CurrencyID(NAME_None)
		, CurrencyTag(FGameplayTag())
		, bLossRule(false)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Currency")
	int32 Index = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Currency")
	FName CurrencyID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Currency")
	FGameplayTag CurrencyTag;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Currency")
	bool bLossRule;

	virtual uint32 GetKey() const override
	{
		return Index;
	}
};
