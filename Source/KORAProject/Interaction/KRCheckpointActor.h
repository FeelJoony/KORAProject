#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractableActorBase.h"
#include "KRCheckpointActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

/**
 * 체크포인트 액터 (휴식 장소)
 *
 * - InteractableActorBase 상속
 * - 상호작용 시 GA_Rest 활성화
 * - 플레이어 스폰 위치 저장
 * - Save UI 표시
 */
UCLASS()
class KORAPROJECT_API AKRCheckpointActor : public AInteractableActorBase
{
	GENERATED_BODY()

public:
	AKRCheckpointActor();

	virtual void BeginPlay() override;

	/** 체크포인트 저장 (GA_Rest에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SaveCheckpoint();

	/** 플레이어 스폰 Transform 반환 */
	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	FTransform GetSpawnTransform() const;

	/** 휴식 완료 시 호출 (GA_Rest에서 호출) */
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void OnRestComplete();

protected:
	virtual void OnAbilityActivated(UGameplayAbility* Ability) override;

	/** 플레이어 스폰 오프셋 (체크포인트 기준) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint|Spawn")
	FVector SpawnOffset = FVector(100.f, 0.f, 0.f);

	/** 스폰 시 플레이어가 바라볼 방향 (체크포인트 기준 Yaw 회전) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint|Spawn")
	float SpawnYawOffset = 180.f;

	/** 활성화 이펙트 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Checkpoint|FX")
	TObjectPtr<UNiagaraSystem> ActivationEffect;

	/** 상시 재생 이펙트 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint|FX")
	TObjectPtr<UNiagaraComponent> IdleEffectComponent;

	/** 활성화 사운드 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Checkpoint|Sound")
	TObjectPtr<USoundBase> ActivationSound;

	/** 체크포인트 식별 태그 (선택적) */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Checkpoint")
	FName CheckpointTag;

private:
	/** Save UI 브로드캐스트 */
	void BroadcastSaveUI();

	/** 이미 활성화된 체크포인트인지 */
	bool bIsActivated = false;
};
