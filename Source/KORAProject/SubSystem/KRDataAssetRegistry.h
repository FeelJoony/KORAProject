#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "KRDataAssetRegistry.generated.h"

class UKRSoundSubsystem;
class UKREffectSubsystem;
class UKRSoundDefinition;
class UKREffectDefinition;
class UKRWorldEventData;
class UDataTable;

DECLARE_LOG_CATEGORY_EXTERN(LogKRDataAssetRegistry, Log, All);

/**
 * DataAsset 등록 결과
 */
USTRUCT(BlueprintType)
struct FKRDataAssetRegistrationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 SoundDefinitionsRegistered = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 EffectDefinitionsRegistered = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 WorldEventsRegistered = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 TotalRegistered = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 FailedCount = 0;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> FailedAssets;
};

/**
 * DataAsset Registry Subsystem
 * - DataTable에서 DataAsset을 로드하여 각 Subsystem에 등록
 * - GameInstance Subsystem으로 게임 전체 수명 동안 유지
 */
UCLASS()
class KORAPROJECT_API UKRDataAssetRegistry : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * DataTablesSubsystem에서 자동으로 DataAsset 등록
	 * - Config 없이 경로 기반으로 자동 인식
	 * - Experience 초기화 이후 호출 됨
	 * @return 등록 결과
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	FKRDataAssetRegistrationResult RegisterAllFromDataTablesSubsystem();

	/**
	 * Sound Definition DataTable 등록
	 * @param DataTable - FKRSoundDefinitionDataStruct 구조의 DataTable
	 * @return 등록된 개수
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	int32 RegisterSoundDefinitionsFromTable(UDataTable* DataTable);

	/**
	 * Effect Definition DataTable 등록
	 * @param DataTable - FKREffectDefinitionDataStruct 구조의 DataTable
	 * @return 등록된 개수
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	int32 RegisterEffectDefinitionsFromTable(UDataTable* DataTable);

	/**
	 * World Event DataTable 등록
	 * @param DataTable - FKRWorldEventDataStruct 구조의 DataTable
	 * @return 등록된 개수
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	int32 RegisterWorldEventsFromTable(UDataTable* DataTable);

	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	bool RegisterSoundDefinition(UKRSoundDefinition* SoundDef);

	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	bool RegisterEffectDefinition(UKREffectDefinition* EffectDef);

	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	bool IsInitialized() const { return bIsInitialized; }

	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	int32 GetRegisteredSoundCount() const { return RegisteredSoundCount; }

	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	int32 GetRegisteredEffectCount() const { return RegisteredEffectCount; }

	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	int32 GetRegisteredWorldEventCount() const { return RegisteredWorldEventCount; }

	UFUNCTION(BlueprintCallable, Category = "KR|DataAssetRegistry")
	UKRWorldEventData* GetWorldEventData(const FGameplayTag& EventTag) const;

private:
	UPROPERTY()
	bool bIsInitialized = false;

	UPROPERTY()
	int32 RegisteredSoundCount = 0;

	UPROPERTY()
	int32 RegisteredEffectCount = 0;

	UPROPERTY()
	int32 RegisteredWorldEventCount = 0;
	
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UKRWorldEventData>> WorldEventDataMap;
};
