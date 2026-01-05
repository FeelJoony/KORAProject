#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "LevelTransitionDataStruct.generated.h"

/**
 * 레벨 전환 시 필요한 데이터
 * CSV RowName: ObjectiveTag (예: "NPC.Clara", "Enemy.Boss.Dead")
 */
USTRUCT(BlueprintType)
struct KORAPROJECT_API FLevelTransitionData : public FTableRowBase
{
	GENERATED_BODY()

	FLevelTransitionData()
		: LevelNameStringTableKey(NAME_None)
		, FadeOutDuration(1.5f)
		, FadeInDuration(1.0f)
		, LevelEnterSoundTag(FGameplayTag::EmptyTag)
	{}

	// 레벨 이름 (StringTable 키)
	// 예: ST_UIBase_Info_Office → StringTable에서 "본부장실" / "Director's Office"
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FName LevelNameStringTableKey;

	// 페이드 아웃 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float FadeOutDuration;

	// 페이드 인 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float FadeInDuration;

	// 레벨 진입 사운드 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FGameplayTag LevelEnterSoundTag;
};
