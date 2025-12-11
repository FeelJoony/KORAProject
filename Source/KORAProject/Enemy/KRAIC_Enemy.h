#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Interface/AICInterface.h"
#include "KRAIC_Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAlertEnd);

UCLASS()
class KORAPROJECT_API AKRAIC_Enemy : public AAIController, public IAICInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void Patrol() const override;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void Alert() const override;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void ChasePlayer() const override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void SenseRange() const override;

	UPROPERTY(BlueprintAssignable, Category = "AI")
	FOnAlertEnd OnAlertEnd;
};
