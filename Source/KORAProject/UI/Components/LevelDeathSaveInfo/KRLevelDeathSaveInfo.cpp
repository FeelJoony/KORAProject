// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Components/LevelDeathSaveInfo/KRLevelDeathSaveInfo.h"
#include "CommonTextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameplayTag/KRSoundTag.h"
#include "Internationalization/StringTable.h"
#include "SubSystem/KRSoundSubsystem.h"

void UKRLevelDeathSaveInfo::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetVisibility(ESlateVisibility::Collapsed);
	
	if (!StringTableAsset.IsNull())
	{
		CachedStringTable = StringTableAsset.LoadSynchronous();
	}

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& Subsystem = UGameplayMessageSubsystem::Get(World);

		InfoMessageListener = Subsystem.RegisterListener<FKRUIMessage_Info>(
			FKRUIMessageTags::SaveDeathLevelInfo(),
			this,
			&UKRLevelDeathSaveInfo::HandleInfoMessage
		);
	}

	bIsShowing = false;
}

void UKRLevelDeathSaveInfo::NativeDestruct()
{
	if (InfoMessageListener.IsValid() && GetWorld())
	{
		UGameplayMessageSubsystem::Get(GetWorld()).UnregisterListener(InfoMessageListener);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoHideTimerHandle);
	}

	Super::NativeDestruct();
}

void UKRLevelDeathSaveInfo::HandleInfoMessage(FGameplayTag Channel, const FKRUIMessage_Info& Message)
{
	ShowInfo(Message.Context, Message.StringTableKey);
}

void UKRLevelDeathSaveInfo::ShowInfo(EInfoContext Context, const FString& StringTableKey)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(AutoHideTimerHandle);
	}
	
	FString KeyToUse;
	switch (Context)
	{
	case EInfoContext::SaveGame:
		KeyToUse = SaveGameKey;
		break;
	case EInfoContext::Death:
		KeyToUse = DeathKey;
		break;
	case EInfoContext::LevelTransition:
		KeyToUse = StringTableKey;
		break;
	default:
		KeyToUse = StringTableKey;
		break;
	}

	UpdateInfoText(KeyToUse);
	SetVisibility(ESlateVisibility::HitTestInvisible);
	bIsShowing = true;

	if (ShowAnim)
	{
		PlayAnimation(ShowAnim);
	}

	if (Context == EInfoContext::LevelTransition)
	{
		if (UWorld* World = GetWorld())
		{
			if (UKRSoundSubsystem* SoundSubsystem =
				World->GetSubsystem<UKRSoundSubsystem>())
			{
				FVector Location = FVector::ZeroVector;

				if (APlayerController* PC = World->GetFirstPlayerController())
				{
					if (APawn* Pawn = PC->GetPawn())
					{
						Location = Pawn->GetActorLocation();
					}
				}

				SoundSubsystem->PlaySoundByTag(
					KRTAG_SOUND_UI_MAPTRANSITION,
					Location,
					nullptr,
					true
				);
			}
		}
	}
	
	if (AutoHideDuration > 0.f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			AutoHideTimerHandle,
			this,
			&UKRLevelDeathSaveInfo::OnAutoHideTimeout,
			AutoHideDuration,
			false
		);
	}
}

void UKRLevelDeathSaveInfo::HideInfo()
{
	if (!bIsShowing)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UKRSoundSubsystem* SoundSubsystem =
			World->GetSubsystem<UKRSoundSubsystem>())
		{
			SoundSubsystem->StopSoundByTag(
				KRTAG_SOUND_UI_MAPTRANSITION,
				0.2f
			);
		}
	}
	
	if (HideAnim)
	{
		PlayAnimation(HideAnim);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}

	bIsShowing = false;
}

void UKRLevelDeathSaveInfo::OnAutoHideTimeout()
{
	HideInfo();
}

void UKRLevelDeathSaveInfo::UpdateInfoText(const FString& StringTableKey)
{
	if (!InfoText)
	{
		return;
	}

	FString TablePath = StringTableAsset.ToSoftObjectPath().GetAssetPathString();
	FText LocalizedText = FText::FromStringTable(*TablePath, StringTableKey);
	InfoText->SetText(LocalizedText);
}
