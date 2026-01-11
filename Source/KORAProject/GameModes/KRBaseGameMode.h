// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KRBaseGameMode.generated.h"

/**
 *
 */
UCLASS()
class KORAPROJECT_API AKRBaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKRBaseGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;

	const class UKRPawnData* GetPawnDataForController(const AController* Controller) const;

protected:
	void RegisterDataAssets();
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	void HandleMatchAssignmentIfNotExpectionOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const class UKRExperienceDefinition* CurrentExperience);

public:
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void RequestRespawn(AController* Controller);

	// 리스폰 시퀀스 시작 (GA_Death에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void StartRespawnSequence(AController* Controller, float Delay = 4.0f);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	// Transform 기반 리스폰 지원
	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	// 리스폰 타이머 콜백
	UFUNCTION()
	void OnRespawnTimerExpired();

private:
	FTimerHandle RespawnTimerHandle;
	TWeakObjectPtr<AController> PendingRespawnController;

	// 리스폰 시 로딩 화면 처리
	void WaitAndHideLoadingScreen();
	void CheckStreamingAndHideLoading();
	FTimerHandle LoadingHideTimerHandle;
};
