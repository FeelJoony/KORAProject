#pragma once

#include "CoreMinimal.h"
#include "InteractableActorBase.h"
#include "GameplayTagContainer.h"
#include "KRQuestItemPickup.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class KORAPROJECT_API AKRQuestItemPickup : public AInteractableActorBase
{
	GENERATED_BODY()

public:
	AKRQuestItemPickup();

	UFUNCTION(BlueprintCallable, Category = "Quest Item")
	void InitializeQuestItem(FGameplayTag InItemTag);

	UFUNCTION(BlueprintCallable, Category = "Quest Item")
	void PickupItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Item")
	FGameplayTag QuestItemTag;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ItemMesh;

private:
	bool bIsPickedUp = false;
};
