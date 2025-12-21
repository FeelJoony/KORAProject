#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "KREffectDefinition.generated.h"

/**
 * 이펙트 시스템 타입
 */
UENUM(BlueprintType)
enum class EKREffectSystemType : uint8
{
	Niagara,
	Cascade
};

/**
 * 이펙트 품질 레벨
 */
UENUM(BlueprintType)
enum class EKREffectQualityLevel : uint8
{
	Low,
	Medium,
	High,
	Epic
};

/**
 * 이펙트 풀링 설정
 */
USTRUCT(BlueprintType)
struct FKREffectPoolingSettings
{
	GENERATED_BODY()

	// 풀링 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pooling")
	bool bUsePooling = true;

	// 사전 생성할 인스턴스 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pooling", meta = (EditCondition = "bUsePooling"))
	int32 PreallocatedCount = 5;

	// 최대 풀 크기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pooling", meta = (EditCondition = "bUsePooling"))
	int32 MaxPoolSize = 20;

	// 최대 동시 활성 인스턴스 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pooling")
	int32 MaxConcurrentInstances = 10;
};

/**
 * 이펙트 스폰 설정
 */
USTRUCT(BlueprintType)
struct FKREffectSpawnSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	bool bAttachToParent = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (EditCondition = "bAttachToParent"))
	FName AttachSocketName = NAME_None;

	// 로컬 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FVector LocationOffset = FVector::ZeroVector;

	// 로컬 회전 오프셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FRotator RotationOffset = FRotator::ZeroRotator;

	// 스케일
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	FVector Scale = FVector::OneVector;

	// 자동 파괴 (이펙트 종료 시)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	bool bAutoDestroy = true;

	// 수명 (0이면 이펙트의 기본 수명 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.0"))
	float Lifetime = 0.0f;
};

/**
 * 이펙트 정의 - Niagara 이펙트를 정의
 */
UCLASS(BlueprintType)
class KORAPROJECT_API UKREffectDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 이펙트 식별 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FGameplayTag EffectTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	EKREffectSystemType EffectSystemType = EKREffectSystemType::Niagara;

	// EffectSystemType이 Niagara일 때 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (EditCondition = "EffectSystemType == EKREffectSystemType::Niagara", EditConditionHides))
	TSoftObjectPtr<UNiagaraSystem> NiagaraEffect;

	// EffectSystemType이 Cascade일 때 사용
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (EditCondition = "EffectSystemType == EKREffectSystemType::Cascade", EditConditionHides))
	TSoftObjectPtr<UParticleSystem> CascadeEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FGameplayTagContainer CategoryTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FKREffectSpawnSettings SpawnSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FKREffectPoolingSettings PoolingSettings;
};
