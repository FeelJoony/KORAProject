#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeTaskBase.h"
#include "KRAIStateTree_PlayGameplayAbilityTask.generated.h"

UENUM(BlueprintType)
enum class EKRAI_PlayGameplayAbilityTaskActivateType : uint8
{
	SingleUse	UMETA(DisplayName = "Single Use"),
	Continuous	UMETA(DisplayName = "Continuous")
};

UCLASS(Category = "KRAI", meta = (DisplayName = "Play Enemy GameplayAbility", ToolTip = "A task that play enemy GameplayAbility", Keywords = "Ability, Task, AI, KRAI"))
class KORAPROJECT_API UKRAIStateTree_PlayGameplayAbilityTask : public UKRAIStateTreeTaskBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_PlayGameplayAbilityTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<class AKRAIEnemyController> AIController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter", meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer AbilityTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameter", meta = (AllowPrivateAccess = "true"))
	EKRAI_PlayGameplayAbilityTaskActivateType ActivateType;

	struct FGameplayAbilitySpec* ActivateSpec = nullptr;
};
