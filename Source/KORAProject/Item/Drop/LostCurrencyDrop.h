#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LostCurrencyDrop.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class KORAPROJECT_API ALostCurrencyDrop : public AActor
{
	GENERATED_BODY()

public:
	ALostCurrencyDrop();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Collision")
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY()
	bool bCollected = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
