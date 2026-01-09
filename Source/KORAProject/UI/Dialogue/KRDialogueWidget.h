#pragma once

#include "CommonActivatableWidget.h"
#include "Data/DialogueDataStruct.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KRDialogueWidget.generated.h"

class UCommonTextBlock;
class UStringTable;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueCompleted, FGameplayTag, NPCTag);

UCLASS()
class KORAPROJECT_API UKRDialogueWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool IsDialogueActive() const { return bDialogueActive; }

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	bool IsVoicePlaying() const;

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueCompleted OnDialogueCompleted;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> DialogueText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UStringTable> DialogueStringTable;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FDialogueDataStruct CurrentDialogueData;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	int32 CurrentLineIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bDialogueActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FName DialogueRouteName = "Quest";

private:
	void DisplayCurrentLine();
	void EndDialogue();
	void PlayCurrentVoice();
	void StopCurrentVoice();

	FText GetTextFromStringTable(FName Key) const;

	void OnDialogueStartMessage(FGameplayTag Channel, const FDialogueDataStruct& DialogueData);

	UFUNCTION()
	void OnVoiceFinished();
	
	FGameplayMessageListenerHandle DialogueListenerHandle;

	UPROPERTY()
	TObjectPtr<UAudioComponent> CurrentVoiceComponent;
};	