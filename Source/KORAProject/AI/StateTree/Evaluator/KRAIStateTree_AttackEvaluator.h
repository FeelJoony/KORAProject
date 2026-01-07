#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeEvaluatorBase.h"
#include "KRAIStateTree_AttackEvaluator.generated.h"

UCLASS()
class KORAPROJECT_API UKRAIStateTree_AttackEvaluator : public UKRAIStateTreeEvaluatorBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_AttackEvaluator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking = false;
};
