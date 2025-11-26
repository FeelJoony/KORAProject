#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Interface/TableKey.h"
#include "TutorialDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FTutorialDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()
	
	FTutorialDataStruct()
		: Duration(0.f)
		, PauseGame(false)
		, OffsetX(0)
		, OffsetY(0)
	{
	}

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	// FName RowName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FString PopupType;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float Duration;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	bool PauseGame;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FName StringKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float OffsetX;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	float OffsetY;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemData)
	FName CompletionTrigger;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(StringKey);
	}
};
