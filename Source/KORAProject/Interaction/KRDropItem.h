#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "KRDropItem.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USphereComponent;

UCLASS()
class KORAPROJECT_API AKRDropItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AKRDropItem();

	virtual void Tick(float DeltaTime) override;
	
	void InitializeDrop(FGameplayTag InItemTag, int32 InQuantity, AActor* InTargetActor);

protected:
	virtual void BeginPlay() override;

	void EnablePickup();
	
	void GiveItemToPlayer();
	
	void SpawnPickupVFX();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> RootSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> ItemVFXComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float PickupDelay = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float FlySpeed = 800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float PickupRange = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> PickupSuccessVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TMap<FGameplayTag, TObjectPtr<UNiagaraSystem>> RarityVFXMap; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TObjectPtr<UNiagaraSystem> DefaultVFX;

private:
	FGameplayTag ItemTag;
	int32 Quantity;
	
	UPROPERTY()
	TWeakObjectPtr<AActor> TargetPlayer;

	bool bCanPickup;
	
	bool bIsConsumed;
};