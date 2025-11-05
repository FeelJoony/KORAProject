#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Data/BaseItemData.h"
#include "UObject/NoExportTypes.h"
#include "KRBaseItem.generated.h"

/**
 * 모든 아이템의 공통 부모
 * 데이터테이블 기반으로 UI/사용에 필요한 최소 정보만 캐싱합니다
 */
UCLASS(Blueprintable, BlueprintType)
class KORAPROJECT_API UKRBaseItem : public UObject
{
	GENERATED_BODY()

public:
	UKRBaseItem();

	/* 데이터 초기화 (DT Row 기반) */
	void Initialize(int32 InIndex, UDataTable* InTable);

protected:
	/* ===== 캐싱된 최소 정보 ===== */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "기본 정보")
	FName CachedItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	FText CachedDisplayName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (MultiLine = true))
	FText CachedDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> CachedIcon;

	/* 스택 수량 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "스택")
	int32 CurrentStackCount = 1;

	/* 데이터 참조용 키 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "데이터")
	int32 CachedDataIndex = -1;

	/* 어떤 DataTable에서 로드했는지 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "데이터")
	UDataTable* SourceDataTable = nullptr;

public:
	/* ===== Getters ===== */
	UFUNCTION(BlueprintPure, Category = "아이템")
	FName GetItemID() const { return CachedItemID; }

	UFUNCTION(BlueprintPure, Category = "아이템")
	FText GetDisplayName() const { return CachedDisplayName; }

	UFUNCTION(BlueprintPure, Category = "아이템")
	FText GetDescription() const { return CachedDescription; }

	UFUNCTION(BlueprintPure, Category = "아이템")
	UTexture2D* GetIcon() const { return CachedIcon.LoadSynchronous(); }

	/* 스택 */
	UFUNCTION(BlueprintCallable, Category = "아이템")
	void AddStack(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "아이템")
	bool RemoveStack(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "아이템")
	int32 GetStackCount() const { return CurrentStackCount; }

	UFUNCTION(BlueprintPure, Category = "아이템")
	int32 GetMaxStackCount() const;

	/* 참조 기반 실제 데이터 접근 */
	const FBaseItemData* GetBaseData() const;
};
