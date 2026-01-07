#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_DebugFormattedText.generated.h"

UENUM(BlueprintType, Category = "Debug")
enum class EKRAI_DebugParamType : uint8
{
	Float		UMETA(DisplayName = "Float"),
	Int			UMETA(DisplayName = "Int"),
	String		UMETA(DisplayName = "String"),
	Name		UMETA(DisplayName = "Name"),
	Text		UMETA(DisplayName = "Text"),
	Bool		UMETA(DisplayName = "Bool"),
};

USTRUCT(BlueprintType, Category = "Debug")
struct FKRAI_DebugFormatParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FName Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	EKRAI_DebugParamType Type = EKRAI_DebugParamType::Float;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float FloatValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FString StringValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FName NameValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	FText TextValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	bool BoolValue = false;
};

UCLASS(meta = (DisplayName = "Debug Formatted Text", Category = "KRAI", Description = "Displays a formatted debug text at an actor's location (or origin)"))
class KORAPROJECT_API UKRAIStateTree_DebugFormattedText : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	FText FormatText = NSLOCTEXT("KRAI", "DebugFmt", "Type Something {Value}");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task")
	TArray<FKRAI_DebugFormatParam> Params;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task", meta = (DisplayName = "Print To Screen"))
	bool PrintToScreen = false;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const override
	{
		return NSLOCTEXT("KRAI_STEHealth", "HealthEvaluatorDesc", "Display Formatted Debug Text at Actor Location");
	}

	virtual FName GetIconName() const
	{
		return FName("Info_Circle");
	}
	
#endif

protected:
	EStateTreeRunStatus EnterState(struct FStateTreeExecutionContext& Context, const struct FStateTreeTransitionResult& Transition) override;
};
