#include "Player/KRPlayerController.h"
#include "Player/KRPlayerState.h"
#include "GAS/KRAbilitySystemComponent.h"

AKRPlayerController::AKRPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AKRPlayerController::OnUnPossess()
{
	if (APawn* PawnUnPossess = GetPawn())
	{
		if (UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponent())
		{
			if (KRASC->GetAvatarActor() == PawnUnPossess)
			{
				KRASC->SetAvatarActor(nullptr);
			}
		}
	}
	Super::OnUnPossess();
}

void AKRPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UKRAbilitySystemComponent* KRASC = GetKRAbilitySystemComponent())
	{
		KRASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

AKRPlayerState* AKRPlayerController::GetKRPlayerState() const
{
	return CastChecked<AKRPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UKRAbilitySystemComponent* AKRPlayerController::GetKRAbilitySystemComponent() const
{
	const AKRPlayerState* KRPS = GetKRPlayerState();
	return (KRPS ? KRPS->GetKRAbilitySystemComponent() : nullptr);
}
