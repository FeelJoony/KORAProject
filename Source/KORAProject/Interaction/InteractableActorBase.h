#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "InteractableActorBase.generated.h"

class USphereComponent;

UCLASS()
class KORAPROJECT_API AInteractableActorBase : public AActor
{
	GENERATED_BODY()
	
public:
	AInteractableActorBase();

	virtual void BeginPlay() override;

	void GiveTagToActor(AActor* TargetActor, FGameplayTag Tag);
	void RemoveTagFromActor(AActor* TargetActor, FGameplayTag Tag);

	void OnActorEnterRange(AActor* TargetActor);
	void OnActorExitRange(AActor* TargetActor);

	void ShowInteractionUI();
	void HideInteractionUI();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = KRAbility)
	FGameplayTag InteractAbilityTag;

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractCollision;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 BodyIndex);
};
