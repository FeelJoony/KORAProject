#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "DialogueDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FDialogueDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FDialogueDataStruct()
		: Index(0)
		, NPCTag(FGameplayTag())
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dialogue)
	int32 Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dialogue)
	FGameplayTag NPCTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Dialogue)
	TArray<FName> DialogueTextKeys;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Dialogue)
	TArray<TSoftObjectPtr<USoundBase>> VoiceAssets;

	virtual uint32 GetKey() const override
	{
		return Index;
	}
};
