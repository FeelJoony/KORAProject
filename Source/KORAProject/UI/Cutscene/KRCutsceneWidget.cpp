#include "KRCutsceneWidget.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Internationalization/StringTable.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MediaSoundComponent.h"
#include "Styling/SlateBrush.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UKRCutsceneWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CurrentTimingIndex = -1;
	bCutscenePlaying = false;
	bSubtitleVisible = true;
}

void UKRCutsceneWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MediaPlayer = NewObject<UMediaPlayer>(this);
	if (MediaPlayer)
	{
		MediaPlayer->OnMediaOpened.AddDynamic(this, &UKRCutsceneWidget::OnMediaOpened);
		MediaPlayer->OnEndReached.AddDynamic(this, &UKRCutsceneWidget::OnMediaEndReached);
		MediaPlayer->OnMediaOpenFailed.AddDynamic(this, &UKRCutsceneWidget::OnMediaOpenFailed);
	}

	MediaTexture = NewObject<UMediaTexture>(this);
	if (MediaTexture)
	{
		MediaTexture->SetMediaPlayer(MediaPlayer);
		MediaTexture->UpdateResource();
	}

	if (CutsceneImage && MediaTexture)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(MediaTexture);
		Brush.ImageSize = FVector2D(1920.0f, 1080.0f);
		CutsceneImage->SetBrush(Brush);
	}

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				MediaSoundComponent = NewObject<UMediaSoundComponent>(Pawn);
				if (MediaSoundComponent)
				{
					MediaSoundComponent->RegisterComponent();
					MediaSoundComponent->SetMediaPlayer(MediaPlayer);
				}
			}
		}
	}
}

void UKRCutsceneWidget::NativeDestruct()
{
	StopCutscene();

	if (MediaPlayer)
	{
		MediaPlayer->OnMediaOpened.RemoveDynamic(this, &UKRCutsceneWidget::OnMediaOpened);
		MediaPlayer->OnEndReached.RemoveDynamic(this, &UKRCutsceneWidget::OnMediaEndReached);
		MediaPlayer->OnMediaOpenFailed.RemoveDynamic(this, &UKRCutsceneWidget::OnMediaOpenFailed);
	}

	if (MediaSoundComponent)
	{
		MediaSoundComponent->SetMediaPlayer(nullptr);
		if (MediaSoundComponent->IsRegistered())
		{
			MediaSoundComponent->UnregisterComponent();
		}
		MediaSoundComponent = nullptr;
	}

	Super::NativeDestruct();
}

void UKRCutsceneWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (!bCutscenePlaying && DefaultMediaSource)
	{
		PlayDefaultCutscene();
	}
}

void UKRCutsceneWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (bCutscenePlaying)
	{
		StopCutscene();
	}
}

void UKRCutsceneWidget::PlayDefaultCutscene()
{
	if (!MediaPlayer || !DefaultMediaSource)
	{
		UE_LOG(LogTemp, Warning, TEXT("KRCutsceneWidget: DefaultMediaSource or MediaPlayer not set"));
		return;
	}

	PlayCutsceneWithTimings(DefaultMediaSource, SubtitleTimings);
}

void UKRCutsceneWidget::PlayCutsceneWithTimings(UMediaSource* MediaSource, const TArray<FSubtitleTiming>& InSubtitleTimings)
{
	if (!MediaPlayer || !MediaSource)
	{
		UE_LOG(LogTemp, Error, TEXT("KRCutsceneWidget::PlayCutsceneWithTimings - MediaPlayer or MediaSource is NULL!"));
		return;
	}

	SubtitleTimings = InSubtitleTimings;
	CurrentTimingIndex = -1;
	bCutscenePlaying = true;

	MediaPlayer->OpenSource(MediaSource);
	HideSubtitle();

	if (SubtitleTimings.Num() > 0)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				SubtitleTimerHandle,
				this,
				&UKRCutsceneWidget::CheckSubtitleTiming,
				0.1f,
				true
			);
		}
	}

	OnCutsceneStarted.Broadcast();
}

void UKRCutsceneWidget::StopCutscene()
{
	if (!bCutscenePlaying)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SubtitleTimerHandle);
	}

	if (MediaPlayer)
	{
		MediaPlayer->Close();
	}

	bCutscenePlaying = false;
	CurrentTimingIndex = -1;

	OnCutsceneEnded.Broadcast();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			Router->CloseRoute(CutsceneRouteName);
		}
	}
}

void UKRCutsceneWidget::HideSubtitle()
{
	bSubtitleVisible = false;
	if (SubtitleText)
	{
		SubtitleText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UKRCutsceneWidget::ShowSubtitle()
{
	bSubtitleVisible = true;
	if (SubtitleText)
	{
		SubtitleText->SetVisibility(ESlateVisibility::Visible);
	}
}

FText UKRCutsceneWidget::GetTextFromStringTable(FName Key) const
{
	if (!SubtitleStringTable)
	{
		return FText::FromName(Key);
	}

	return FText::FromStringTable(SubtitleStringTable->GetStringTableId(), Key.ToString());
}

void UKRCutsceneWidget::OnMediaOpened(FString OpenedUrl)
{
	if (MediaPlayer && bCutscenePlaying)
	{
		MediaPlayer->Play();
	}
}

void UKRCutsceneWidget::OnMediaEndReached()
{
	StopCutscene();
}

void UKRCutsceneWidget::OnMediaOpenFailed(FString FailedUrl)
{
	StopCutscene();
}

void UKRCutsceneWidget::CheckSubtitleTiming()
{
	if (!bCutscenePlaying || !MediaPlayer)
	{
		return;
	}

	const FTimespan CurrentTime = MediaPlayer->GetTime();
	const float CurrentSeconds = static_cast<float>(CurrentTime.GetTotalSeconds());
	int32 NewTimingIndex = -1;
	for (int32 i = SubtitleTimings.Num() - 1; i >= 0; --i)
	{
		if (CurrentSeconds >= SubtitleTimings[i].StartTime)
		{
			NewTimingIndex = i;
			break;
		}
	}

	if (NewTimingIndex != CurrentTimingIndex)
	{
		CurrentTimingIndex = NewTimingIndex;

		if (CurrentTimingIndex >= 0 && SubtitleTimings.IsValidIndex(CurrentTimingIndex))
		{
			const FName& SubtitleKey = SubtitleTimings[CurrentTimingIndex].SubtitleKey;
			if (SubtitleKey.IsNone() || SubtitleKey.IsEqual(NAME_None))
			{
				HideSubtitle();
			}
			else
			{
				FText Text = GetTextFromStringTable(SubtitleKey);
				if (SubtitleText)
				{
					SubtitleText->SetText(Text);
				}
				ShowSubtitle();
			}
		}
		else
		{
			HideSubtitle();
		}
	}
}
