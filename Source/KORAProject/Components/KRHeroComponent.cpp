#include "Components/KRHeroComponent.h"
#include "Components/KRPawnExtensionComponent.h"
#include "Components/Input/KRInputComponent.h"
#include "Player/KRLocalPlayer.h"
#include "Player/KRPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Player/KRPlayerState.h"
#include "Data/DataAssets/KRPawnData.h"
#include "Data/DataAssets/DataAsset_InputConfig.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GameplayTag/KRInputTag.h"
#include "GameplayTag/KRStateTag.h"
#include "Components//GameFrameworkComponentManager.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Components/KRCameraComponent.h"
#include "Camera/KRCameraMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRHeroComponent)

const FName UKRHeroComponent::NAME_ActorFeatureName("Hero");

UKRHeroComponent::UKRHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	bReadyToBindInputs = false;
}

bool UKRHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);
	
    APawn* Pawn = GetPawn<APawn>();
	
    if (!CurrentState.IsValid() && DesiredState == KRTAG_STATE_INIT_SPAWNED)
    {
        if (Pawn)
        {
            return true;
        }
    }
    else if (CurrentState == KRTAG_STATE_INIT_SPAWNED && DesiredState == KRTAG_STATE_INIT_DATAAVAILABLE)
    {
        if (!GetPlayerState<AKRPlayerState>())
        {
            return false;
        }

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
    	const bool bIsBot = Pawn->IsBotControlled();
    	
        if (bIsLocallyControlled && !bIsBot)
        {
            AKRPlayerController* KRPC = GetController<AKRPlayerController>();
            if (!Pawn->InputComponent || !KRPC || !KRPC->GetLocalPlayer())
            {
                return false;
            }
        }
        return true;
    }
    else if (CurrentState == KRTAG_STATE_INIT_DATAAVAILABLE && DesiredState == KRTAG_STATE_INIT_DATAINITIALIZED)
    {
    	AKRPlayerState* KRPS = GetPlayerState<AKRPlayerState>();
    	
    	bool bPawnExtInitialized = Manager->HasFeatureReachedInitState(Pawn, UKRPawnExtensionComponent::NAME_ActorFeatureName, KRTAG_STATE_INIT_DATAINITIALIZED);
    	bool bPawnExtReady = Manager->HasFeatureReachedInitState(Pawn, UKRPawnExtensionComponent::NAME_ActorFeatureName, KRTAG_STATE_INIT_GAMEPLAYREADY);
    
    	// 둘 중 하나라도 참이면 통과
    	bool bPawnExtPass = bPawnExtInitialized || bPawnExtReady;
    	
    	return KRPS && bPawnExtPass;
    }
    else if (CurrentState == KRTAG_STATE_INIT_DATAINITIALIZED && DesiredState == KRTAG_STATE_INIT_GAMEPLAYREADY)
    {
        return true;
    }

    return false;
}

void UKRHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == KRTAG_STATE_INIT_DATAAVAILABLE && DesiredState == KRTAG_STATE_INIT_DATAINITIALIZED)
	{
		APawn* Pawn = GetPawn<APawn>();
		AKRPlayerState* KRPS = GetPlayerState<AKRPlayerState>();
		if (!ensure(Pawn && KRPS))
		{
			return;
		}

		const UKRPawnData* PawnData = nullptr;

		if (UKRPawnExtensionComponent* PawnExtComp = UKRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UKRPawnData>();
			PawnExtComp->InitializeAbilitySystem(KRPS->GetKRAbilitySystemComponent(), KRPS);
		}
		
		if(AKRPlayerController* KRPC = GetController<AKRPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		if (PawnData)
		{
			if (UKRCameraComponent* CameraComponent = UKRCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void UKRHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UKRPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == KRTAG_STATE_INIT_DATAINITIALIZED || Params.FeatureState == KRTAG_STATE_INIT_GAMEPLAYREADY)
		{
			CheckDefaultInitialization();
		}
	}
}

void UKRHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = {
		KRTAG_STATE_INIT_SPAWNED,
		KRTAG_STATE_INIT_DATAAVAILABLE,
		KRTAG_STATE_INIT_DATAINITIALIZED,
		KRTAG_STATE_INIT_GAMEPLAYREADY
	};

	ContinueInitStateChain(StateChain);
}

void UKRHeroComponent::OnRegister()
{
	Super::OnRegister();
	RegisterInitStateFeature();
}

void UKRHeroComponent::BeginPlay()
{
	Super::BeginPlay();
	BindOnActorInitStateChanged(UKRPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	ensure(TryToChangeInitState(KRTAG_STATE_INIT_SPAWNED));
	CheckDefaultInitialization();
}

void UKRHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UKRHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) return;

	const AKRPlayerController* KRPC = GetController<AKRPlayerController>();
	check(KRPC);

	const UKRLocalPlayer* KRLP = Cast<UKRLocalPlayer>(KRPC->GetLocalPlayer());
	check(KRLP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = KRLP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem)

	//Subsystem->ClearAllMappings();
	
	if (const UKRPawnExtensionComponent* PawnExtComp = UKRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UKRPawnData* PawnData = PawnExtComp->GetPawnData<UKRPawnData>())
		{
			if (const UDataAsset_InputConfig* InputConfig = PawnData->InputConfig)
			{
				if (InputConfig->DefaultMappingContext)
				{
					Subsystem->AddMappingContext(InputConfig->DefaultMappingContext, 0);
				}

				UKRInputComponent* KRIC = CastChecked<UKRInputComponent>(PlayerInputComponent);

				KRIC->BindNativeInputAction(InputConfig, KRTAG_INPUT_MOVE,ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
				KRIC->BindNativeInputAction(InputConfig, KRTAG_INPUT_LOOK,ETriggerEvent::Triggered, this, &ThisClass::Input_Look);

				KRIC->BindAbilityInputAction(InputConfig, this, &ThisClass::Input_AbilityInputPressed, &UKRHeroComponent::Input_AbilityInputReleased);
			}
		}
	}
}

void UKRHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) return;
	
	AController* PC = Pawn->GetController();

	if (PC)
	{
		const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
		LastMoveInput = MovementVector;
		
		const FRotator MovementRotation(0.f, PC->GetControlRotation().Yaw, 0.f);

		if (MovementVector.Y != 0.f)
		{
			const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		}

		if (MovementVector.X != 0.f)
		{
			const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}

void UKRHeroComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	LastLookInput = InputActionValue.Get<FVector2D>();
	
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) return;

	AKRPlayerState* KRPS = GetPlayerState<AKRPlayerState>();
	if (UKRAbilitySystemComponent* ASC = KRPS->GetKRAbilitySystemComponent())
	{
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("State.Acting.LockOn")))
		{
			LastLookInput = InputActionValue.Get<FVector2D>();
			return;
		}
	}
	
	const FVector2D LookAxisVector = LastLookInput;

	if (LookAxisVector.X != 0.f)
	{
		Pawn->AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		Pawn->AddControllerPitchInput(LookAxisVector.Y);
	}
}

void UKRHeroComponent::Input_AbilityInputPressed(FGameplayTag InputTag)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) return;

	const AKRPlayerState* KRPS = Pawn->GetPlayerState<AKRPlayerState>();
	if (KRPS)
	{
		if (UKRAbilitySystemComponent* KRASC = KRPS->GetKRAbilitySystemComponent())
		{
			KRASC->AbilityInputTagPressed(InputTag);
		}
	}
}

void UKRHeroComponent::Input_AbilityInputReleased(FGameplayTag InputTag)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) return;

	const AKRPlayerState* KRPS = Pawn->GetPlayerState<AKRPlayerState>();
	if (KRPS)
	{
		if (UKRAbilitySystemComponent* KRASC = KRPS->GetKRAbilitySystemComponent())
		{
			KRASC->AbilityInputTagReleased(InputTag);
		}
	}
}

TSubclassOf<UKRCameraMode> UKRHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn) return nullptr;

	if (UKRPawnExtensionComponent* PawnExtComp = UKRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UKRPawnData* PawnData = PawnExtComp->GetPawnData<UKRPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}
