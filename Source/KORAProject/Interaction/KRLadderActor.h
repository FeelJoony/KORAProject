#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableActorBase.h"
#include "KRLadderActor.generated.h"

UCLASS()
class KORAPROJECT_API AKRLadderActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:
	AKRLadderActor();

	virtual	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder")
	TObjectPtr<UStaticMeshComponent> LadderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder")
	float LadderYawOffset = 0.f;
	
	UFUNCTION(BlueprintPure, Category = "Ladder")
	FTransform GetSnapTransform(const FVector& InActorLocation) const;

	UFUNCTION(BlueprintPure, Category = "Ladder")
	FVector GetTopLocation() const;

	UFUNCTION(BlueprintPure, Category = "Ladder")
	FVector GetBottomLocation() const;
	
protected:
	virtual void OnAbilityActivated(UGameplayAbility* InAbility) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder")
	TObjectPtr<USceneComponent> TopPoint;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder")
	TObjectPtr<USceneComponent> BottomPoint;
};
