#pragma once

#include "CoreMinimal.h"
#include "KRAIStateTreeEvaluatorBase.h"
#include "KRAIStateTree_MainBossEvaluator.generated.h"

UENUM(BlueprintType)
enum class EUseAbilityPriority : uint8
{
	Low,
	Normal,
	Middle,
	High,
	Critical
};

USTRUCT(BlueprintType)
struct FMainBossEvaluatorAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	EUseAbilityPriority Priority;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	bool bIsSingleExecuteAbility = false;
	
	float CooldownLeft = 0.f;

	bool CanExecute() const { return CooldownLeft <= 0.f; }
};

USTRUCT(BlueprintType)
struct FMainBossEvaluatorAbilityInfoContainer
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	TArray<FMainBossEvaluatorAbilityInfo> AbilityInfos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter)
	float EntryPageHealthPercent;
};

UCLASS()
class KORAPROJECT_API UKRAIStateTree_MainBossEvaluator : public UKRAIStateTreeEvaluatorBase
{
	GENERATED_BODY()

public:
	UKRAIStateTree_MainBossEvaluator(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void TreeStart(FStateTreeExecutionContext& Context) override;
	void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
	void TreeStop(FStateTreeExecutionContext& Context) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output, meta = (Output))
	TSubclassOf<class UGameplayAbility> SelectedAbilityClass = nullptr;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Parameter, meta = (AllowPrivateAccess = "true"))
	TArray<FMainBossEvaluatorAbilityInfoContainer> AbilityInfoContainers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AKREnemyPawn> Actor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class AKRAIEnemyController> AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Output, meta = (Output))
	int32 CurrentPhase = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (Input))
	float CurrentHealthPercent = 0.f;
};
