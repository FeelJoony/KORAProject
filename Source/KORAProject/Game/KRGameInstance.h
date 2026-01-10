// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KRGameInstance.generated.h"


UCLASS()
class KORAPROJECT_API UKRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	/** 마지막으로 저장된 체크포인트 태그 (PlayerStart용) */
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	FName LastCheckpointTag;

	/** 마지막으로 저장된 체크포인트 Transform (정확한 위치 스폰용) */
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	FTransform LastCheckpointTransform;

	/** 체크포인트가 설정되었는지 여부 */
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	bool bHasCheckpoint = false;

	/** 체크포인트 저장 */
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SaveCheckpoint(const FTransform& SpawnTransform, FName CheckpointTag = NAME_None);

	/** 체크포인트 초기화 */
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void ClearCheckpoint();

protected:
	virtual void Init() override;
};
