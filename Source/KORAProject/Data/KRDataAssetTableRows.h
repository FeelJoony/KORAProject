#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "DataAssets/KRSoundDefinition.h"
#include "DataAssets/KREffectDefinition.h"
#include "DataAssets/KRWorldEventData.h"
#include "KRDataAssetTableRows.generated.h"

/**
 * Sound Definition DataTable Row
 * - GameplayTag로 캐싱 및 조회 지원
 */
USTRUCT(BlueprintType)
struct FSoundDefinitionDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	// 사운드 식별 태그 (CacheDataTable 키로 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FGameplayTag SoundTag;

	// Sound Definition DataAsset 참조
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TSoftObjectPtr<UKRSoundDefinition> SoundDefinition;

	// 설명 (에디터용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FString Description;

	// 활성화 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	bool bEnabled = true;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(SoundTag);
	}
};

/**
 * Effect Definition DataTable Row
 * - GameplayTag로 캐싱 및 조회 지원
 */
USTRUCT(BlueprintType)
struct FEffectDefinitionDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	// 이펙트 식별 태그 (CacheDataTable 키로 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FGameplayTag EffectTag;

	// Effect Definition DataAsset 참조
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TSoftObjectPtr<UKREffectDefinition> EffectDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	bool bEnabled = true;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(EffectTag);
	}
};

/**
 * World Event DataTable Row
 * - GameplayTag로 캐싱 및 조회 지원
 */
USTRUCT(BlueprintType)
struct FWorldEventDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	// 월드 이벤트 식별 태그 (CacheDataTable 키로 사용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	FGameplayTag EventTag;

	// World Event DataAsset 참조
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	TSoftObjectPtr<UKRWorldEventData> WorldEventData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WorldEvent")
	bool bEnabled = true;

	virtual uint32 GetKey() const override
	{
		return GetTypeHash(EventTag);
	}
};
