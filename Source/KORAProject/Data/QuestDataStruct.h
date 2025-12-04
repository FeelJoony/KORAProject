#pragma once

#include "CoreMinimal.h"
#include "Interface/TableKey.h"
#include "QuestDataStruct.generated.h"

USTRUCT(BlueprintType)
struct KORAPROJECT_API FQuestDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()
	
public:
	FQuestDataStruct()
		: Index(0)
		, QuestName(TEXT_NULL)
		, Description(TEXT_NULL)
		, StateTreeDefinitionPath(TEXT_NULL)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	int32 Index;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	FString QuestName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	FString Description;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Quest)
	FName StateTreeDefinitionPath;

	virtual uint32 GetKey() const override
	{
		return Index;
	}
};
