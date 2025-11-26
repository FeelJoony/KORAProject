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
		
		if (Pawn->IsLocallyControlled() && !Pawn->IsBotControlled())
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
		
		return KRPS && Manager->HasFeatureReachedInitState(Pawn, UKRPawnExtensionComponent::Name_ActorFeatureName, KRTAG_STATE_INIT_DATAINITIALIZED);
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
			//PawnExtComp->InitializeAbilitySystem(KRPS->GetKR)
		}
		
		if(AKRPlayerController* KRPC = GetController<AKRPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
}

void UKRHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UKRPawnExtensionComponent::Name_ActorFeatureName)
	{
		if (Params.FeatureState == KRTAG_STATE_INIT_DATAINITIALIZED)
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
	BindOnActorInitStateChanged(UKRPawnExtensionComponent::Name_ActorFeatureName, FGameplayTag(), false);

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

	Subsystem->ClearAllMappings();

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

	AKRPlayerController* KRPC = Pawn->GetController<AKRPlayerController>();

	if (KRPC)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
	}
}

void UKRHeroComponent::Input_Look(const FInputActionValue& InputActionValue)
{
}

void UKRHeroComponent::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
}

void UKRHeroComponent::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
}
