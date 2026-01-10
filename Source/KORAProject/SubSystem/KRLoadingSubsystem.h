#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "KRLoadingSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnLoadingScreenHidden);

UCLASS()
class KORAPROJECT_API UKRLoadingSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	static UKRLoadingSubsystem& Get();

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void ShowLoadingScreen();

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void HideLoadingScreen();

	UFUNCTION(BlueprintPure, Category = "Loading")
	bool IsLoadingScreenVisible() const { return bIsVisible; }

	FOnLoadingScreenHidden OnLoadingScreenHidden;
	
private:
	TSharedPtr<SWidget> LoadingScreenSlateWidget;
	bool bIsVisible = false;
};
