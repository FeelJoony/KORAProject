#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableActorBase.h"
#include "GameplayTagContainer.h"
#include "KRChestActor.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class UAnimMontage;
class UAnimSequence;
class USoundBase;
class AKRDropItem;

UCLASS()
class KORAPROJECT_API AKRChestActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:
	AKRChestActor();
	virtual void BeginPlay() override;

protected:
	virtual void OnAbilityActivated(UGameplayAbility* Ability) override;
	
	void OpenChest(AActor* Interactor);
	void SpawnLoot(AActor* Interactor);
	FGameplayTag GetRandomItemTag(int32& OutQuantity);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<USceneComponent> DefaultSceneRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<UBoxComponent> BlockCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	TObjectPtr<USkeletalMeshComponent> ChestMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Animation")
	TObjectPtr<UAnimSequence> ChestOpenAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Animation")
	TObjectPtr<UAnimMontage> PlayerOpenMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Sound")
	TObjectPtr<USoundBase> OpenSound;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Chest|Loot")
	int32 MinDropCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Loot")
	int32 MaxDropCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Chest|Loot")
	TSubclassOf<AKRDropItem> DropItemClass;

	bool bIsOpened;
};