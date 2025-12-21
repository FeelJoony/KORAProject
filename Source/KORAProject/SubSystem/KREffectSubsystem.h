#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/DataAssets/KREffectDefinition.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "KREffectSubsystem.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class UParticleSystem;
class UParticleSystemComponent;

/**
 * 이펙트 풀 정보
 */
USTRUCT()
struct FKREffectPool
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag EffectTag;

	UPROPERTY()
	EKREffectSystemType EffectSystemType = EKREffectSystemType::Niagara;

	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> AvailableNiagaraComponents;

	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> ActiveNiagaraComponents;

	UPROPERTY()
	TArray<TObjectPtr<UParticleSystemComponent>> AvailableCascadeComponents;

	UPROPERTY()
	TArray<TObjectPtr<UParticleSystemComponent>> ActiveCascadeComponents;

	UPROPERTY()
	int32 MaxPoolSize = 20;

	int32 GetAvailableCount() const
	{
		return EffectSystemType == EKREffectSystemType::Niagara ? AvailableNiagaraComponents.Num() : AvailableCascadeComponents.Num();
	}

	int32 GetActiveCount() const
	{
		return EffectSystemType == EKREffectSystemType::Niagara ? ActiveNiagaraComponents.Num() : ActiveCascadeComponents.Num();
	}
};

/**
 * 활성 이펙트 인스턴스 정보
 */
USTRUCT()
struct FKRActiveEffectInstance
{
	GENERATED_BODY()

	UPROPERTY() TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;
	UPROPERTY() TObjectPtr<UParticleSystemComponent> CascadeComponent = nullptr;
	UPROPERTY() EKREffectSystemType EffectSystemType = EKREffectSystemType::Niagara;

	UPROPERTY()
	FGameplayTag EffectTag;

	UPROPERTY()
	float SpawnTime = 0.0f;

	UPROPERTY()
	float Lifetime = 0.0f;

	UPROPERTY()
	bool bAutoDestroy = true;

	bool IsValid() const
	{
		if (EffectSystemType == EKREffectSystemType::Niagara)
		{
			return NiagaraComponent != nullptr && NiagaraComponent->IsValidLowLevel();
		}
		return CascadeComponent != nullptr && CascadeComponent->IsValidLowLevel();
	}

	bool IsActive() const
	{
		if (EffectSystemType == EKREffectSystemType::Niagara)
		{
			return NiagaraComponent != nullptr && NiagaraComponent->IsActive();
		}
		return CascadeComponent != nullptr && CascadeComponent->IsActive();
	}

	UFXSystemComponent* GetFXComponent() const
	{
		if (EffectSystemType == EKREffectSystemType::Niagara)
		{
			return NiagaraComponent;
		}
		return CascadeComponent;
	}

	bool ShouldDestroy(float CurrentTime) const;
};

/**
 * 이펙트 서브시스템 - 이펙트 풀링, 컬링, 품질 관리
 */
UCLASS()
class KORAPROJECT_API UKREffectSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

	// UFXSystemComponent는 Niagara와 Cascade의 공통 부모 클래스
	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	UFXSystemComponent* SpawnEffectByTag(const FGameplayTag& EffectTag, const FTransform& Transform, AActor* Owner = nullptr);

	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	UFXSystemComponent* SpawnEffectAttached(const FGameplayTag& EffectTag, USceneComponent* AttachToComponent, FName SocketName = NAME_None, const FVector& LocationOffset = FVector::ZeroVector, const FRotator& RotationOffset = FRotator::ZeroRotator);

	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	UFXSystemComponent* SpawnEffect(const UKREffectDefinition* EffectDef, const FTransform& Transform, AActor* Owner = nullptr);

	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	void StopEffectByTag(const FGameplayTag& EffectTag, bool bImmediate = false);

	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	void StopAllEffects(bool bImmediate = false);

	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	void StopEffectsByCategory(const FGameplayTagContainer& CategoryTags, bool bImmediate = false);

	// 이펙트 정의 등록
	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	void RegisterEffectDefinition(UKREffectDefinition* EffectDef);

	// 이펙트 정의 가져오기
	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	UKREffectDefinition* GetEffectDefinition(const FGameplayTag& EffectTag) const;

	// 이펙트 품질 레벨 설정
	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	void SetEffectQualityLevel(EKREffectQualityLevel QualityLevel);

	// 최대 이펙트 수 설정
	UFUNCTION(BlueprintCallable, Category = "KR|Effect")
	void SetMaxEffectCount(int32 MaxCount);

protected:
	// 등록된 이펙트 정의
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UKREffectDefinition>> EffectDefinitions;

	UPROPERTY()
	TMap<FGameplayTag, FKREffectPool> EffectPools;

	UPROPERTY()
	TArray<FKRActiveEffectInstance> ActiveEffects;

	UPROPERTY()
	EKREffectQualityLevel CurrentQualityLevel = EKREffectQualityLevel::High;

	UPROPERTY()
	int32 MaxActiveEffects = 100;

private:
	// 풀에서 Niagara 컴포넌트 가져오기 (또는 새로 생성)
	UNiagaraComponent* AcquireNiagaraComponentFromPool(const UKREffectDefinition* EffectDef);

	// 풀에서 Cascade 컴포넌트 가져오기 (또는 새로 생성)
	UParticleSystemComponent* AcquireCascadeComponentFromPool(const UKREffectDefinition* EffectDef);

	// Niagara 컴포넌트를 풀로 반환
	void ReturnNiagaraComponentToPool(const FGameplayTag& EffectTag, UNiagaraComponent* Component);

	// Cascade 컴포넌트를 풀로 반환
	void ReturnCascadeComponentToPool(const FGameplayTag& EffectTag, UParticleSystemComponent* Component);

	// 활성 이펙트 인스턴스를 풀로 반환
	void ReturnInstanceToPool(FKRActiveEffectInstance& Instance);

	// 풀 초기화
	void InitializePool(const UKREffectDefinition* EffectDef);

	void CleanupInactiveEffects();
	bool CanSpawnEffect(const UKREffectDefinition* EffectDef, const FVector& Location) const;

	// 중요도가 낮은 이펙트 제거 (최대 수 초과 시)
	void RemoveLowPriorityEffects(int32 NumToRemove);

	void ConfigureNiagaraComponent(UNiagaraComponent* Component, const UKREffectDefinition* EffectDef, const FTransform& Transform);
	void ConfigureCascadeComponent(UParticleSystemComponent* Component, const UKREffectDefinition* EffectDef, const FTransform& Transform);
};
