#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRPortalActor.generated.h"

/**
 * 레벨에 직접 배치하는 포탈 액터
 * - ObjectiveTag로 활성화 조건 설정
 * - UserFacingPath로 이동 대상 설정
 */
UCLASS()
class KORAPROJECT_API AKRPortalActor : public AActor
{
	GENERATED_BODY()

public:
	AKRPortalActor();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal|Activation")
	FGameplayTag ActivationObjectiveTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal|Destination")
	FString TargetUserFacingPath;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> PortalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UNiagaraComponent> PortalVFX;

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
	void ActivatePortalIfMatch(FGameplayTag Channel);
	
	// 테스트용: 에디터에서 수동으로 포탈 활성화
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Portal|Testing")
	void ActivatePortalForTesting();

private:
	FGameplayMessageListenerHandle ListenerHandle;
	bool bIsActivated = false;
};
