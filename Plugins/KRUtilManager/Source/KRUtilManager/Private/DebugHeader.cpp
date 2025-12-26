#include "DebugHeader.h"

DEFINE_LOG_CATEGORY(LogTool);

void FDebugHeader::Print(const FString& Message, const FColor& Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
	}
}

void FDebugHeader::PrintLog(const FString& Message)
{
	UE_LOG(LogTool, Warning, TEXT("%s"), *Message);
}

EAppReturnType::Type FDebugHeader::ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning)
{
	if (bShowMsgAsWarning)
	{
		return FMessageDialog::Open(MsgType, FText::FromString(Message), FText::FromString(TEXT("Warning")));
	}
	else
	{
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}

void FDebugHeader::ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo NotifyInfo(FText::FromString(Message));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = 7.f;

	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}
