#include "KRDialogueWidget.h"
#include "CommonTextBlock.h"
#include "CommonInputModeTypes.h"
#include "Internationalization/StringTable.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "SubSystem/KRUIRouterSubsystem.h"
#include "SubSystem/KRQuestSubsystem.h"
#include "GameplayTag/KREventTag.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/KRSettingsSubsystem.h"

void UKRDialogueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	bDialogueActive = false;
	CurrentLineIndex = 0;
}

void UKRDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MsgSubsystem = UGameplayMessageSubsystem::Get(World);
		if (MsgSubsystem.IsValidLowLevel())
		{
			DialogueListenerHandle = MsgSubsystem.RegisterListener<FDialogueDataStruct>(
				KRTAG_EVENT_DIALOGUE_START,
				this,
				&ThisClass::OnDialogueStartMessage
			);
		}
	}
}

void UKRDialogueWidget::NativeDestruct()
{
	if (DialogueListenerHandle.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayMessageSubsystem::Get(World).UnregisterListener(DialogueListenerHandle);
		}
	}

	Super::NativeDestruct();
}

void UKRDialogueWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	SetIsFocusable(true);
	if (ULocalPlayer* LP = GetOwningLocalPlayer())
	{
		if (UKRUIInputSubsystem* InputSubsystem = LP->GetSubsystem<UKRUIInputSubsystem>())
		{
			InputSubsystem->BindRow(this, InputSubsystem->Rows.Select, FSimpleDelegate::CreateUObject(this, &ThisClass::AdvanceDialogue), false, true);
		}
	}
}

void UKRDialogueWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	StopCurrentVoice();

	if (bDialogueActive)
	{
		EndDialogue();
	}
}

void UKRDialogueWidget::AdvanceDialogue()
{
	if (!bDialogueActive)
	{
		return;
	}
	
	StopCurrentVoice();

	CurrentLineIndex++;

	if (CurrentLineIndex >= CurrentDialogueData.DialogueTextKeys.Num())
	{
		EndDialogue();
	}
	else
	{
		DisplayCurrentLine();
	}
}

void UKRDialogueWidget::DisplayCurrentLine()
{
	if (!bDialogueActive || CurrentLineIndex >= CurrentDialogueData.DialogueTextKeys.Num())
	{
		return;
	}

	FName CurrentKey = CurrentDialogueData.DialogueTextKeys[CurrentLineIndex];
	FText LineText = GetTextFromStringTable(CurrentKey);

	if (DialogueText)
	{
		DialogueText->SetText(LineText);
	}
	PlayCurrentVoice();
}

void UKRDialogueWidget::EndDialogue()
{
	StopCurrentVoice();

	bDialogueActive = false;
	CurrentLineIndex = 0;
	OnDialogueCompleted.Broadcast(CurrentDialogueData.NPCTag);

	if (UWorld* World = GetWorld())
	{
		UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(World);

		const bool bAlreadyTalked = QuestSubsystem.HasTalkedToNPCForCurrentQuest(CurrentDialogueData.NPCTag);
		if (CurrentDialogueData.NPCTag.IsValid() && !bAlreadyTalked)
		{
			QuestSubsystem.MarkNPCDialogueForQuest(CurrentDialogueData.NPCTag);
			QuestSubsystem.NotifyNPCDialogueComplete(CurrentDialogueData.NPCTag);
		}
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UKRUIRouterSubsystem* Router = GI->GetSubsystem<UKRUIRouterSubsystem>())
		{
			Router->CloseRoute(DialogueRouteName);
		}
	}
}

FText UKRDialogueWidget::GetTextFromStringTable(FName Key) const
{
	if (!DialogueStringTable)
	{
		return FText::FromName(Key);
	}

	return FText::FromStringTable(DialogueStringTable->GetStringTableId(), Key.ToString());
}

void UKRDialogueWidget::OnDialogueStartMessage(FGameplayTag Channel, const FDialogueDataStruct& DialogueData)
{
	if (DialogueData.DialogueTextKeys.Num() == 0)
	{
		return;
	}

	CurrentDialogueData = DialogueData;
	CurrentLineIndex = 0;
	bDialogueActive = true;

	DisplayCurrentLine();
}

void UKRDialogueWidget::PlayCurrentVoice()
{
	StopCurrentVoice();
	if (!CurrentDialogueData.VoiceAssets.IsValidIndex(CurrentLineIndex))
	{
		return;
	}

	TSoftObjectPtr<USoundBase> VoiceAssetPtr = CurrentDialogueData.VoiceAssets[CurrentLineIndex];
	if (VoiceAssetPtr.IsNull())
	{
		return;
	}

	USoundBase* VoiceSound = VoiceAssetPtr.LoadSynchronous();
	if (!VoiceSound)
	{
		return;
	}

	float VolumeMultiplier = 1.0f;
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UKRSettingsSubsystem* Settings = GI->GetSubsystem<UKRSettingsSubsystem>())
		{
			VolumeMultiplier = Settings->GetAppliedSettings().Audio.MasterVolume;
		}
	}

	CurrentVoiceComponent = UGameplayStatics::SpawnSound2D(this, VoiceSound, VolumeMultiplier, 1.0f, 0.0f, nullptr, false, false);
	if (CurrentVoiceComponent)
	{
		CurrentVoiceComponent->OnAudioFinished.AddDynamic(this, &UKRDialogueWidget::OnVoiceFinished);
		CurrentVoiceComponent->Play();
	}
}

void UKRDialogueWidget::StopCurrentVoice()
{
	if (CurrentVoiceComponent && CurrentVoiceComponent->IsPlaying())
	{
		CurrentVoiceComponent->OnAudioFinished.RemoveDynamic(this, &UKRDialogueWidget::OnVoiceFinished);
		CurrentVoiceComponent->Stop();
	}
	CurrentVoiceComponent = nullptr;
}

bool UKRDialogueWidget::IsVoicePlaying() const
{
	return CurrentVoiceComponent && CurrentVoiceComponent->IsPlaying();
}

void UKRDialogueWidget::OnVoiceFinished()
{
	AdvanceDialogue();
}