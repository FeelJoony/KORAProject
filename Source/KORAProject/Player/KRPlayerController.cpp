#include "Player/KRPlayerController.h"
#include "Camera/KRPlayerCameraManager.h"
#include "Player/KRPlayerState.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Subsystem/KRQuestSubsystem.h"
#include "SubSystem/KRUIInputSubsystem.h"
#include "Interaction/InteractableActorBase.h"
#include "Engine/LocalPlayer.h"

AKRPlayerController::AKRPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AKRPlayerCameraManager::StaticClass();
}

void AKRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 레벨 로드 시 UIInputSubsystem 리셋 (모든 레벨 전환 방식에 대응)
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UKRUIInputSubsystem* UIInputSubsystem = LP->GetSubsystem<UKRUIInputSubsystem>())
		{
			UIInputSubsystem->ForceResetUIMode();
		}
	}

	// InputMode를 게임 모드로 설정
	SetShowMouseCursor(false);
	FInputModeGameOnly GameOnlyMode;
	SetInputMode(GameOnlyMode);
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

void AKRPlayerController::SetCurrentInteractableActor(AInteractableActorBase* InActor)
{
	CurrentInteractableActor = InActor;
}

AInteractableActorBase* AKRPlayerController::GetCurrentInteractableActor() const
{
	return CurrentInteractableActor.Get();
}
