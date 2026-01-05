#pragma once

#include "CommonActivatableWidget.h"
#include "GameplayTagContainer.h"
#include "KRCutsceneWidget.generated.h"

class UCommonTextBlock;
class UStringTable;
class UImage;
class UMediaPlayer;
class UMediaSource;
class UMediaTexture;
class UMediaSoundComponent;

USTRUCT(BlueprintType)
struct FSubtitleTiming
{
	GENERATED_BODY()

	FSubtitleTiming()
		: StartTime(0.0f)
		, SubtitleKey(NAME_None)
	{
	}

	FSubtitleTiming(float InTime, FName InKey)
		: StartTime(InTime)
		, SubtitleKey(InKey)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle")
	float StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Subtitle")
	FName SubtitleKey;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutsceneStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCutsceneEnded);

UCLASS()
class KORAPROJECT_API UKRCutsceneWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION(BlueprintCallable, Category = "Cutscene")
	void PlayDefaultCutscene();
	UFUNCTION(BlueprintCallable, Category = "Cutscene")
	void PlayCutsceneWithTimings(UMediaSource* MediaSource, const TArray<FSubtitleTiming>& InSubtitleTimings);
	UFUNCTION(BlueprintCallable, Category = "Cutscene")
	void StopCutscene();

	UFUNCTION(BlueprintCallable, Category = "Cutscene")
	void HideSubtitle();
	UFUNCTION(BlueprintCallable, Category = "Cutscene")
	void ShowSubtitle();

	UFUNCTION(BlueprintPure, Category = "Cutscene")
	bool IsCutscenePlaying() const { return bCutscenePlaying; }

	UFUNCTION(BlueprintPure, Category = "Cutscene")
	int32 GetCurrentSubtitleIndex() const { return CurrentTimingIndex; }

	UPROPERTY(BlueprintAssignable, Category = "Cutscene")
	FOnCutsceneStarted OnCutsceneStarted;
	UPROPERTY(BlueprintAssignable, Category = "Cutscene")
	FOnCutsceneEnded OnCutsceneEnded;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> CutsceneImage;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCommonTextBlock> SubtitleText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cutscene")
	TObjectPtr<UStringTable> SubtitleStringTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cutscene|Media")
	TObjectPtr<UMediaPlayer> MediaPlayer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cutscene|Media")
	TObjectPtr<UMediaTexture> MediaTexture;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cutscene|Media")
	TObjectPtr<UMediaSource> DefaultMediaSource;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Cutscene|Subtitle")
	TArray<FSubtitleTiming> SubtitleTimings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cutscene")
	FName CutsceneRouteName = "Cutscene";

private:
	FText GetTextFromStringTable(FName Key) const;

	UFUNCTION()
	void OnMediaEndReached();
	UFUNCTION()
	void OnMediaOpenFailed(FString FailedUrl);

	void CheckSubtitleTiming();
	
	UPROPERTY()
	TObjectPtr<UMediaSoundComponent> MediaSoundComponent;
	FTimerHandle SubtitleTimerHandle;

	int32 CurrentTimingIndex;
	bool bCutscenePlaying;
	bool bSubtitleVisible;
};
