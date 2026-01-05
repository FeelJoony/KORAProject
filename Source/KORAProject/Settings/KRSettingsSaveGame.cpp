#include "Settings/KRSettingsSaveGame.h"

UKRSettingsSaveGame::UKRSettingsSaveGame()
{
	Audio = FKRAudioSettings();
	Control = FKRControlSettings();
	Gameplay = FKRGameplaySettings();
	SaveVersion = CurrentSaveVersion;
}
