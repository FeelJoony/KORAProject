#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "KRAerialComboEffectLibrary.generated.h"

USTRUCT(BlueprintType)
struct FKRAfterImageSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AfterImage")
	float SpawnInterval = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AfterImage")
	float FadeDuration = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AfterImage")
	FLinearColor Color = FLinearColor(0.5f, 0.7f, 1.0f, 0.8f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AfterImage")
	int32 MaxAfterImages = 5;
};

USTRUCT(BlueprintType)
struct FKRImpactWaveSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float Radius = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float ExpansionSpeed = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float Height = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	FLinearColor Color = FLinearColor(1.0f, 0.5f, 0.2f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	int32 CrackCount = 8;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Impact")
	float CrackLength = 200.0f;
};

USTRUCT(BlueprintType)
struct FKRSlamTrailSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail")
	float Width = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail")
	float Length = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail")
	FLinearColor Color = FLinearColor(1.0f, 0.3f, 0.1f, 1.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trail")
	float GlowIntensity = 3.0f;
};

UCLASS()
class KORAPROJECT_API UKRAerialComboEffectLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 상승 기류 이펙트 스폰
	 * @param WorldContext 월드 컨텍스트
	 * @param NiagaraSystem Niagara 시스템
	 * @param Location 스폰 위치
	 * @param Height 기류 높이
	 * @param Color 이펙트 색상
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|AerialCombo|Effect", meta = (WorldContext = "WorldContext"))
	static UNiagaraComponent* SpawnLaunchWindEffect(
		UObject* WorldContext,
		UNiagaraSystem* NiagaraSystem,
		FVector Location,
		float Height = 500.0f,
		FLinearColor Color = FLinearColor::White
	);
	
	/**
	 * 잔상 이펙트 시작 (캐릭터에 부착)
	 * @param Character 대상 캐릭터
	 * @param NiagaraSystem Niagara 시스템
	 * @param Settings 잔상 설정
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|AerialCombo|Effect")
	static UNiagaraComponent* StartAfterImageEffect(
		ACharacter* Character,
		UNiagaraSystem* NiagaraSystem,
		const FKRAfterImageSettings& Settings
	);
	
	UFUNCTION(BlueprintCallable, Category = "KR|AerialCombo|Effect")
	static void StopAfterImageEffect(UNiagaraComponent* AfterImageComponent);
	
	/**
	 * 슬램 궤적 이펙트 시작
	 * @param Character 대상 캐릭터
	 * @param NiagaraSystem Niagara 시스템
	 * @param Settings 궤적 설정
	 * @param SocketName 부착 소켓 이름
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|AerialCombo|Effect")
	static UNiagaraComponent* StartSlamTrailEffect(
		ACharacter* Character,
		UNiagaraSystem* NiagaraSystem,
		const FKRSlamTrailSettings& Settings,
		FName SocketName = FName("weapon_r")
	);
	
	UFUNCTION(BlueprintCallable, Category = "KR|AerialCombo|Effect")
	static void StopSlamTrailEffect(UNiagaraComponent* TrailComponent);
	
	/**
	 * 착지 충격파 이펙트 스폰
	 * @param WorldContext 월드 컨텍스트
	 * @param NiagaraSystem Niagara 시스템
	 * @param Location 착지 위치
	 * @param Settings 충격파 설정
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|AerialCombo|Effect", meta = (WorldContext = "WorldContext"))
	static UNiagaraComponent* SpawnImpactWaveEffect(
		UObject* WorldContext,
		UNiagaraSystem* NiagaraSystem,
		FVector Location,
		const FKRImpactWaveSettings& Settings
	);

	/**
	 * 바닥 균열 이펙트 스폰
	 * @param WorldContext 월드 컨텍스트
	 * @param NiagaraSystem Niagara 시스템
	 * @param Location 착지 위치
	 * @param Settings 충격파 설정
	 */
	UFUNCTION(BlueprintCallable, Category = "KR|AerialCombo|Effect", meta = (WorldContext = "WorldContext"))
	static UNiagaraComponent* SpawnGroundCrackEffect(
		UObject* WorldContext,
		UNiagaraSystem* NiagaraSystem,
		FVector Location,
		const FKRImpactWaveSettings& Settings
	);

	UFUNCTION(BlueprintPure, Category = "KR|AerialCombo|Utility", meta = (WorldContext = "WorldContext"))
	static FVector GetGroundLocation(UObject* WorldContext, FVector StartLocation, float TraceDistance = 1000.0f);

	/**
	 * 방사형 균열 위치 계산
	 * @param Center 중심점
	 * @param CrackCount 균열 개수
	 * @param CrackLength 균열 길이
	 * @return 균열 끝점 배열
	 */
	UFUNCTION(BlueprintPure, Category = "KR|AerialCombo|Utility")
	static TArray<FVector> CalculateCrackEndPoints(FVector Center, int32 CrackCount, float CrackLength);
};
