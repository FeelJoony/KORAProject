// Copyright (c) 2025 Studio3F. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScannerController.generated.h"

class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;
class USoundBase;

// TODO: Network 
USTRUCT(BlueprintType)
struct FScanRepState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan|Net")
	FVector_NetQuantize100 Center = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan|Net")
	float Radius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan|Net")
	bool bActive = false;
};

UCLASS(Blueprintable)
class SPHEREREVEAL_API AScannerController : public AActor
{
	GENERATED_BODY()
public:
	AScannerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scan")
	UMaterialParameterCollection* ScannerMPC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	FName ParamCenter = TEXT("ScanCenterWS");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	FName ParamRadius = TEXT("ScanRadius");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	FName ParamActive = TEXT("ScanActive");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	FName ParamEditorOverride = TEXT("EditorOverride");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	float StartRadius = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	float ExpandSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	float MaxRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="Scan|Advanced")
	float MaxRadiusFallback = 100000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan|Timing")
	float HoldAtMaxSeconds = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan|Timing")
	float CooldownSeconds = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan")
	AActor* FollowActor = nullptr;

	UFUNCTION(BlueprintCallable, Category="Scan")
	void StartScan();

	UFUNCTION(BlueprintCallable, Category="Scan")
	void StopScan(bool bHardCut = true);
	
	// Spawned Actor
	UPROPERTY(EditAnywhere, Category="Scan|FX")
	bool bAutoSpawnFX = true;

	UPROPERTY(EditAnywhere, Category="Scan|FX")
	TSubclassOf<AActor> ScanFXClass;   // BP_ScanFX
	
	UPROPERTY(Transient)
	AActor* SpawnedFX = nullptr;

	void UpdateFX(bool bActive);
	void BeginCooldown();

	// === SFX (시작 사운드 전용) ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan|SFX")
	bool bEnableSFX = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan|SFX", meta=(ClampMin="0.0", ClampMax="1.0"))
	float SFXVolume = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scan|SFX")
	USoundBase* SFX_Start = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UMaterialParameterCollectionInstance* MPCI = nullptr;
	FVector CachedCenter = FVector::ZeroVector;
	float Radius = 0.f;
	float FXMeshRadius = 50.f;
	bool  bHoldingAtMax = false;
	float HoldTimer = 0.f;

	bool  bCoolingDown = false;
	float CooldownTimer = 0.f;

	void SetVec(FName Name, const FVector& V);
	void SetFloat(FName Name, float V);
	float GetFloat(FName Name) const;
};
