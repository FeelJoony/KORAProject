#pragma once

#include "CoreMinimal.h"
#include "Interface/TableKey.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "ModuleDataStruct.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EModeuleOp : uint8
{
	Add UMETA(DisplayName = "Add (+)"), //합
	Multiply UMETA(DisplayName = "Multiply (%)"), //곱
	Override UMETA(DisplayName = "Override (=)") //덮어쓰기
};

USTRUCT(BlueprintType)
struct KORAPROJECT_API FModuleDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FModuleDataStruct()
		: Magnitude(0.0f)
		, Operation(EModeuleOp::Add)
	{}

public:
	//--------Identity--------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FName DisplayNameKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	TSoftObjectPtr<UTexture2D> Icon;

	//--------Logic--------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
	FGameplayTag TargetStatTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
	float Magnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
	EModeuleOp Operation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Logic")
	FGameplayTagContainer RequiredSlotTags;

public:
	virtual uint32 GetKey() const override { return GetTypeHash(ItemTag); }
};