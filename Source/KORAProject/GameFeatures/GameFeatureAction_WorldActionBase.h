#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_WorldActionBase.generated.h"

UCLASS()
class KORAPROJECT_API UGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureActivating(struct FGameFeatureActivatingContext& Context) override;

	virtual void AddToWorld(const FWorldContext& WorldContext, const struct FGameFeatureStateChangeContext& ChangeContext) PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld, );
};
