#pragma once

#include "CoreMinimal.h"
#include "IDirectoryWatcher.h"
#include "DirectoryWatcherModule.h"
#include "Misc/Paths.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDirectoryChange, Log, All);

DECLARE_DELEGATE(FDirectoryChangeDelegate);

class KORACUSTOMEDITORS_API FDirectoryChangeWatcher
{
public:
	FDirectoryChangeWatcher();
	~FDirectoryChangeWatcher();

	/* Content 폴더 내의 특정 서브 디렉토리를 감시하도록 등록한다. */
	void StartWatchingDirectory(const FString& DirectoryToWatch, FDirectoryChangeDelegate&& InChangeDelegate);

private:
	void OnDirectoryChanged(const TArray<FFileChangeData>& FileChanges);

	FString AbsolutePath;
	FDelegateHandle DirectoryWatcherHandle;
	FDirectoryChangeDelegate ChangeDelegate;
};
