#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRCutSceneActor.generated.h"

UCLASS()
class KORAPROJECT_API AKRCutSceneActor : public AActor
{
	GENERATED_BODY()

public:
	AKRCutSceneActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CutScene|Activation")
	FGameplayTag ActivationObjectiveTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CutScene")
	FName RouteToOpen;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> TriggerBox;

	// ===== 메시지 리스닝 =====
	
	UFUNCTION()
	void OnKilledMonster(FGameplayTag Channel, const struct FKillMonsterMessage& Message);

	UFUNCTION()
	void OnItemAdded(FGameplayTag Channel, const struct FAddItemMessage& Message);

	UFUNCTION()
	void OnLocationEntered(FGameplayTag Channel, const struct FQuestEventTriggerBoxMessage& Message);

	UFUNCTION()
	void OnNPCDialogueCompleted(FGameplayTag Channel, const struct FQuestNPCDialogueCompleteMessage& Message);
	
	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnLoadingScreenHidden();
	
	UFUNCTION()
	void ActivateCutSceneIfMatch(FGameplayTag Channel);
	
private:
	bool bIsActivated = false;
	FGameplayMessageListenerHandle ListenerHandle;
};
