#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KRStarPatternLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "KRStarEffectSpawner.generated.h"

/**
 * 별 이펙트 스폰 설정
 * Star Effect 파라미터: DrawAlpha, DrawDuration, GlowIntensity, LineColor, LineWidth, Points
 */
USTRUCT(BlueprintType)
struct FKRStarEffectSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float DrawDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float LineWidth = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FLinearColor LineColor = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float GlowIntensity = 5.0f;
};

UCLASS()
class KORAPROJECT_API UKRStarEffectSpawner : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 별 모양 라인 이펙트 스폰
	 * @param WorldContext 월드 컨텍스트
	 * @param PatternData 별 패턴 데이터 (KRStarPatternLibrary로 생성)
	 * @param Settings 이펙트 설정
	 * @return 생성된 Niagara 컴포넌트
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarEffect", meta = (WorldContext = "WorldContext"))
	static UNiagaraComponent* SpawnStarLineEffect(
		UObject* WorldContext,
		const FKRStarPatternData& PatternData,
		const FKRStarEffectSettings& Settings
	);

	/**
	 * 타겟 액터 주변에 별 이펙트 스폰
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarEffect", meta = (WorldContext = "WorldContext"))
	static UNiagaraComponent* SpawnStarEffectAroundActor(
		UObject* WorldContext,
		AActor* TargetActor,
		float Radius,
		const FKRStarEffectSettings& Settings,
		float HeightOffset = 50.0f
	);

	/**
	 * 개별 라인 이펙트 스폰 (Star 방식 - Points 2개)
	 * Star Effect와 동일한 파라미터 사용으로 일관된 비주얼
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarEffect", meta = (WorldContext = "WorldContext"))
	static UNiagaraComponent* SpawnSingleLineEffect(
		UObject* WorldContext,
		UNiagaraSystem* NiagaraSystem,
		FVector StartPoint,
		FVector EndPoint,
		FLinearColor Color,
		float Width,
		float DrawDuration = 1.0f,
		float GlowIntensity = 5.0f
	);

	/**
	 * DrawAlpha 업데이트 (0~1)
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarEffect")
	static void UpdateStarEffectReveal(UNiagaraComponent* NiagaraComponent, float Alpha);

	/**
	 * 별 그리기 애니메이션 시작 (타이머 기반)
	 * DrawDuration 동안 0에서 1로 DrawAlpha 애니메이션
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarEffect", meta = (WorldContext = "WorldContext"))
	static void StartStarDrawAnimation(
		UObject* WorldContext,
		UNiagaraComponent* NiagaraComponent,
		float Duration
	);

	/**
	 * Spline VFX 블루프린트 액터를 스폰하여 두 점 사이에 Lightning 생성
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarEffect|Lightning", meta = (WorldContext = "WorldContext"))
	static AActor* SpawnSplineLightningActor(
		UObject* WorldContext,
		TSubclassOf<AActor> SplineVFXClass,
		FVector StartPoint,
		FVector EndPoint,
		float LifeSpan = 2.0f,
		int32 Seed = -1
	);

	/**
	 * 휘는 경로 포인트 생성
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|StarEffect|Lightning")
	static void GenerateCurvedLightningPoints(
		FVector StartPoint,
		FVector EndPoint,
		int32 Seed,
		TArray<FVector>& OutPoints
	);
};
