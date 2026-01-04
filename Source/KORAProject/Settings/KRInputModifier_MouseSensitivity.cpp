#include "Settings/KRInputModifier_MouseSensitivity.h"
#include "Settings/KRSettingsSubsystem.h"
#include "EnhancedPlayerInput.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

FInputActionValue UKRInputModifier_MouseSensitivity::ModifyRaw_Implementation(
	const UEnhancedPlayerInput* PlayerInput,
	FInputActionValue CurrentValue,
	float DeltaTime)
{
	if (!PlayerInput)
	{
		return CurrentValue;
	}
	
	APlayerController* PC = Cast<APlayerController>(PlayerInput->GetOuter());
	if (!PC)
	{
		return CurrentValue;
	}
	
	UGameInstance* GameInstance = PC->GetGameInstance();
	if (!GameInstance)
	{
		return CurrentValue;
	}

	UKRSettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<UKRSettingsSubsystem>();
	if (!SettingsSubsystem)
	{
		return CurrentValue;
	}

	const FKRControlSettings& ControlSettings = SettingsSubsystem->GetAppliedSettings().Control;
	
	FVector2D RawValue = CurrentValue.Get<FVector2D>();
	FVector2D ModifiedValue;
	ModifiedValue.X = RawValue.X * ControlSettings.MouseSensitivityX;
	ModifiedValue.Y = RawValue.Y * ControlSettings.MouseSensitivityY;

	return FInputActionValue(ModifiedValue);
}
