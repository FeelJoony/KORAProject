#include "Core/DirectoryChangeWatcher.h"
//#include "Misc/FileChangeData.h"

DEFINE_LOG_CATEGORY(LogDirectoryChange);

FDirectoryChangeWatcher::FDirectoryChangeWatcher()
{

}

FDirectoryChangeWatcher::~FDirectoryChangeWatcher()
{
	// 모듈이 언로드될 때, 반드시 등록된 디렉토리 감시를 해제해야 합니다.
	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher"));

	if (DirectoryWatcherModule.Get() && DirectoryWatcherHandle.IsValid())
	{
		DirectoryWatcherModule.Get()->UnregisterDirectoryChangedCallback_Handle(AbsolutePath, DirectoryWatcherHandle);
	}
}

void FDirectoryChangeWatcher::StartWatchingDirectory(const FString& DirectoryToWatch, FDirectoryChangeDelegate&& InChangeDelegate)
{
	FDirectoryWatcherModule& DirectoryWatcherModule = FModuleManager::LoadModuleChecked<FDirectoryWatcherModule>(TEXT("DirectoryWatcher"));
	IDirectoryWatcher* DirectoryWatcher = DirectoryWatcherModule.Get();

	if (DirectoryWatcher)
	{
		// 1. 감시할 절대 경로를 만든다.
		if (AbsolutePath.IsEmpty())
		{
			AbsolutePath = FPaths::ProjectContentDir() / DirectoryToWatch;
		}
		
		if (false == ChangeDelegate.IsBound())
		{
			ChangeDelegate = InChangeDelegate;
		}

		UE_LOG(LogDirectoryChange, Log, TEXT("Watching directory: %s"), *AbsolutePath);

		DirectoryWatcher->RegisterDirectoryChangedCallback_Handle(
			AbsolutePath,
			IDirectoryWatcher::FDirectoryChanged::CreateRaw(this, &FDirectoryChangeWatcher::OnDirectoryChanged),
			DirectoryWatcherHandle,
			IDirectoryWatcher::WatchOptions::IncludeDirectoryChanges
		);
	}
}

void FDirectoryChangeWatcher::OnDirectoryChanged(const TArray<FFileChangeData>& FileChanges)
{
	bool bIsChange = false;
	for (const FFileChangeData& Change : FileChanges)
	{
		FString ChangeType = TEXT("UNKNOWN");

		switch (Change.Action)
		{
			case FFileChangeData::FCA_Unknown:
				{
					ChangeType = TEXT("UNKNOWN");
				}
				break;

			case FFileChangeData::FCA_Added:
				{
					ChangeType = TEXT("ADDED");
					if (!bIsChange)
					{
						bIsChange = true;
					}
				}
				break;

			case FFileChangeData::FCA_Modified:
				{
					ChangeType = TEXT("MODIFIED");
					if (!bIsChange)
					{
						bIsChange = true;
					}
				}
				break;

			case FFileChangeData::FCA_Removed:
				{
					ChangeType = TEXT("REMOVED");
					if (!bIsChange)
					{
						bIsChange = true;
					}
				}
				break;

			case FFileChangeData::FCA_RescanRequired:
				{
					ChangeType = TEXT("RESCANREQUIRED");
					if (!bIsChange)
					{
						bIsChange = true;
					}
				}
				break;
		}

		UE_LOG(LogDirectoryChange, Log, TEXT("File Change Detected: [%s] - %s"), *ChangeType, *Change.Filename);
	}

	if (bIsChange)
	{
		ChangeDelegate.ExecuteIfBound();
	}
}
