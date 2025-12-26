#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "InteractableActorBase.generated.h"

class USphereComponent;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityActivatedBP, UGameplayAbility*, Ability);

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

	void EndInteractActor();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsInteractable() const { return bIsInteract; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = KRAbility)
	FGameplayTag InteractAbilityTag;

protected:

	UFUNCTION(BlueprintCallable)
	virtual void OnAbilityActivated(UGameplayAbility* Ability);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interact", meta = (AllowPrivateAccess = "true"))
	bool bIsInteract;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnAbilityActivatedBP OnAbilityActivateBP;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractCollision;

	UPROPERTY()
	UAbilitySystemComponent* ObservedASC;

	FDelegateHandle AbilityActivatedHandle;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 BodyIndex);
};
