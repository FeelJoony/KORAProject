#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KRSettingsTypes.h"
#include "KRSettingsSaveGame.generated.h"

UCLASS()
class KORAPROJECT_API UKRSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UKRSettingsSaveGame();
	
	UPROPERTY()
	FKRAudioSettings Audio;
	UPROPERTY()
	FKRControlSettings Control;
	UPROPERTY()
	FKRGameplaySettings Gameplay;
	
	UPROPERTY()
	int32 SaveVersion = 1;

	static constexpr int32 CurrentSaveVersion = 1;
};
