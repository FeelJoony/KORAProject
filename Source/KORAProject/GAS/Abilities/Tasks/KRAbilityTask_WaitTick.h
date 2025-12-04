#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "KRAbilityTask_WaitTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKRWaitTickDelegate, float, DeltaTime);

UCLASS()
class KORAPROJECT_API UKRAbilityTask_WaitTick : public UAbilityTask
{
	GENERATED_BODY()

public:
	UKRAbilityTask_WaitTick(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FKRWaitTickDelegate OnTick;

	UFUNCTION(BlueprintCallable, Category = "Abiltiy|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UKRAbilityTask_WaitTick* WaitTick(UGameplayAbility* OwningAbility, FName TaskInstanceName);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	
};
