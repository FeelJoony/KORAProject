#include "Characters/KRHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Data/DataAsset_InputConfig.h"
#include "Components/Input/KRInputComponent.h"
#include "Data/StartUpData/DataAsset_StartUpDataBase.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/KRGameplayTags.h"
#include "GAS/KRPlayerAbilitySystemComponent.h"
#include "Player/KRPlayerState.h"

AKRHeroCharacter::AKRHeroCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 200.f;
	SpringArm->SocketOffset = FVector(0.f, 55.f, 65.f);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
}

void AKRHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKRHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	checkf(InputConfigDataAsset, TEXT("Forgot assign a data asset"));

	if (ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
		}
	}
	
	UKRInputComponent* KRInputComponent = CastChecked<UKRInputComponent>(PlayerInputComponent);

	KRInputComponent->BindNativeInputAction(InputConfigDataAsset, KRTag_Input_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	KRInputComponent->BindNativeInputAction(InputConfigDataAsset, KRTag_Input_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	
	KRInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
}

void AKRHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;

	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			if (UKRPlayerAbilitySystemComponent* KRASC = Cast<UKRPlayerAbilitySystemComponent>(GetAbilitySystemComponent()))
			{
				LoadedData->GiveToAbilitySystemComponent(KRASC);
			}
		}
	}
}

void AKRHeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AKRPlayerState* KRPS = GetPlayerState<AKRPlayerState>())
	{
		KRPS->InitASCForAvatar(this);
		SetCachedASC(KRPS->GetAbilitySystemComponent());
	}
}

UPawnCombatComponent* AKRHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

void AKRHeroCharacter::Input_Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator MovementRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);

	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}

	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AKRHeroCharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AKRHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	if (UKRPlayerAbilitySystemComponent* KRASC = Cast<UKRPlayerAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		KRASC->OnAbilityInputPressed(InInputTag);
	}

}

void AKRHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
	if (UKRPlayerAbilitySystemComponent* KRASC = Cast<UKRPlayerAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		KRASC->OnAbilityInputReleased(InInputTag);
	}
}
