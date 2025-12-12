#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleVolume.generated.h"

UCLASS()
class KORAPROJECT_API AGrappleVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrappleVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> GrapplePoints;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ChangeWidgetClass(AActor* GrapplePointActor);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void AllClearWidgetClass();
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<AActor> CachedGrapplePoint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UBoxComponent> BoxComponent;

protected:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult
	);
    
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void WidgetBehavior();

private:

	void LineTrace(const FVector& StartLocation, const FRotator& StartRotation);
	
	UPROPERTY(VisibleDefaultsOnly)
	float TraceRange = 5000.f; //수정할 때 KRGA_Grapple도 수정
	UPROPERTY()
	TObjectPtr<class AKRHeroCharacter> CachedPlayer = nullptr;
	UPROPERTY()
	TObjectPtr<class UCameraComponent> CachedCamera = nullptr;

	FTimerHandle Timer;
};
