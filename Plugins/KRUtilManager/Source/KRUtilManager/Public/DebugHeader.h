#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTool, Log, All);

class KRUTILMANAGER_API FDebugHeader
{
public:
	static void Print(const FString& Message, const FColor& Color);
	static void PrintLog(const FString& Message);
	static EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = true);
	static void ShowNotifyInfo(const FString& Message);


};
