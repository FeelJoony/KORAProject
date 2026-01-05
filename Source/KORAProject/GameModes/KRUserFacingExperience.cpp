#include "GameModes/KRUserFacingExperience.h"
#include "CommonSessionSubsystem.h"

UCommonSession_HostSessionRequest* UKRUserFacingExperience::CreateHostingRequest() const
{
	FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();
	if (ExperienceName.Contains(TEXT("/")))
	{
		ExperienceName = FPaths::GetBaseFilename(ExperienceName);
	}

	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

	return Result;
}
