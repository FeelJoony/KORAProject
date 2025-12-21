#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/DataAssets/KRSoundDefinition.h"
#include "Components/AudioComponent.h"
#include "KRSoundSubsystem.generated.h"

class USoundBase;

/**
 * 사운드 인스턴스 정보
 */
USTRUCT()
struct FKRActiveSoundInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComponent = nullptr;

	UPROPERTY()
	FGameplayTag SoundTag;

	UPROPERTY()
	float StartTime = 0.0f;

	UPROPERTY()
	int32 Priority = 0;

	bool IsValid() const { return AudioComponent != nullptr && AudioComponent->IsValidLowLevel(); }
};

/**
 * 사운드 쿨다운 정보
 */
USTRUCT()
struct FKRSoundCooldownInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag SoundTag;

	UPROPERTY()
	float LastPlayTime = 0.0f;

	UPROPERTY()
	float CooldownDuration = 0.0f;
};

/**
 * 사운드 서브시스템 - 사운드 재생, 볼륨, 쿨다운, 동시재생 관리
 */
UCLASS()
class KORAPROJECT_API UKRSoundSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }

	// 사운드 재생 (태그 기반)
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	UAudioComponent* PlaySoundByTag(const FGameplayTag& SoundTag, const FVector& Location, AActor* Owner = nullptr, bool bAutoDestroy = true);

	// 사운드 정의로 직접 재생
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	UAudioComponent* PlaySound(const UKRSoundDefinition* SoundDef, const FVector& Location, AActor* Owner = nullptr, bool bAutoDestroy = true);

	// 사운드 정지
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	void StopSoundByTag(const FGameplayTag& SoundTag, float FadeOutDuration = 0.0f);

	// 모든 사운드 정지
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	void StopAllSounds(float FadeOutDuration = 0.0f);

	// 카테고리별 사운드 정지
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	void StopSoundsByCategory(const FGameplayTagContainer& CategoryTags, float FadeOutDuration = 0.0f);

	// 사운드가 재생 중인지 확인
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	bool IsSoundPlaying(const FGameplayTag& SoundTag) const;

	// 사운드 재생 가능 여부 확인 (쿨다운, 최대 인스턴스 등)
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	bool CanPlaySound(const FGameplayTag& SoundTag) const;

	// 사운드 정의 등록
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	void RegisterSoundDefinition(UKRSoundDefinition* SoundDef);

	// 사운드 정의 가져오기
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	UKRSoundDefinition* GetSoundDefinition(const FGameplayTag& SoundTag) const;

	// 마스터 볼륨 설정
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	void SetMasterVolume(float Volume);

	// 카테고리별 볼륨 설정
	UFUNCTION(BlueprintCallable, Category = "KR|Sound")
	void SetCategoryVolume(const FGameplayTag& CategoryTag, float Volume);

protected:
	// 등록된 사운드 정의
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UKRSoundDefinition>> SoundDefinitions;

	// 활성 사운드 인스턴스
	UPROPERTY()
	TArray<FKRActiveSoundInstance> ActiveSounds;

	// 사운드 쿨다운 정보
	UPROPERTY()
	TArray<FKRSoundCooldownInfo> SoundCooldowns;

	// 카테고리별 볼륨 승수
	UPROPERTY()
	TMap<FGameplayTag, float> CategoryVolumeMultipliers;

	// 마스터 볼륨
	UPROPERTY()
	float MasterVolume = 1.0f;

private:
	// 활성 사운드 정리 (종료된 사운드 제거)
	void CleanupInactiveSounds();

	// 쿨다운 체크
	bool IsOnCooldown(const FGameplayTag& SoundTag) const;

	// 쿨다운 추가
	void AddCooldown(const FGameplayTag& SoundTag, float Duration);

	// 동시 재생 인스턴스 수 확인
	int32 GetActiveInstanceCount(const FGameplayTag& SoundTag) const;

	// 최종 볼륨 계산 (마스터, 카테고리, 거리 등 고려)
	float CalculateFinalVolume(const UKRSoundDefinition* SoundDef, const FVector& Location, const FVector& ListenerLocation) const;

	// 랜덤 피치 계산
	float CalculateRandomPitch(const UKRSoundDefinition* SoundDef) const;

	// AudioComponent 생성 및 설정
	UAudioComponent* CreateAndConfigureAudioComponent(const UKRSoundDefinition* SoundDef, const FVector& Location, AActor* Owner, float Volume, float Pitch);
};
