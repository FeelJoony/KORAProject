#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "KRInputModifier_MouseSensitivity.generated.h"

UCLASS(NotBlueprintable, meta = (DisplayName = "KR Mouse Sensitivity"))
class KORAPROJECT_API UKRInputModifier_MouseSensitivity : public UInputModifier
{
	GENERATED_BODY()

public:
	virtual FInputActionValue ModifyRaw_Implementation(
		const UEnhancedPlayerInput* PlayerInput,
		FInputActionValue CurrentValue,
		float DeltaTime) override;
};
