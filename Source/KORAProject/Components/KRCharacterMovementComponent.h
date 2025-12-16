#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KRCharacterMovementComponent.generated.h"

class UKRAbilitySystemComponent;
class AKRLadderActor;

UENUM(BlueprintType)
enum class EKRMovementMode : uint8
{
	Ground = 0,
	Ladder,
};

UCLASS()
class KORAPROJECT_API UKRCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual float GetMaxSpeed() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartClimbingLadder(const AKRLadderActor* NewLadder);
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopClimbingLadder();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetLadderMounting(bool bMounting);

	UFUNCTION(BlueprintPure, Category = "Movement")
	float GetLadderClimbDirection() const { return GetClimbDirection(); }
	
	const AKRLadderActor* GetCurrentLadder() const { return CurrentLadder; }

protected:
	virtual void BeginPlay() override;
	
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
	void PhysLadder(float deltaTime, int32 Iterations);

	float GetClimbDirection() const;
	
	void SnapToLadderLocation();
	
	void CheckLadderExit(float ClimbDirection);

	bool bIsLadderMounting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder")
	float MaxLadderSpeed = 150.f;
	
	UPROPERTY(Transient)
	mutable TWeakObjectPtr<UKRAbilitySystemComponent> CachedASC;
	
	UPROPERTY(Transient)
	TObjectPtr<const AKRLadderActor> CurrentLadder;

	
};