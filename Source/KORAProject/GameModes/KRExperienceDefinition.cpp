#include "GameModes/KRExperienceDefinition.h"

UKRExperienceDefinition::UKRExperienceDefinition(const FObjectInitializer& ObjectInitializer)
{
}

FPrimaryAssetId UKRExperienceDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("KRExperienceDefinition"), GetPackage()->GetFName());
}
