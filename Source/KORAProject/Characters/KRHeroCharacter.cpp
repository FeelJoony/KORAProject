#include "Characters/KRHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/KRCameraComponent.h"
#include "Components/KRCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/KRHeroComponent.h"
#include "Components/KRPawnExtensionComponent.h"
#include "MotionWarpingComponent.h"

AKRHeroCharacter::AKRHeroCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UKRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CameraComponent = CreateDefaultSubobject<UKRCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.f, 0.f, 75.f));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	
	HeroComponent = CreateDefaultSubobject<UKRHeroComponent>(TEXT("HeroComponent"));

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
}

void AKRHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	PawnExtensionComponent->HandleControllerChanged();

	if (HeroComponent)
	{
		HeroComponent->CheckDefaultInitialization();
	}
}

void AKRHeroCharacter::SetMovementMode_Strafe(bool bStrafe)
{
	if (bStrafe)
	{
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	}
	else
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	}
}

void AKRHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtensionComponent->SetupPlayerInputComponent();

	if (HeroComponent)
	{
		HeroComponent->CheckDefaultInitialization();
	}
}

void AKRHeroCharacter::GetIgnoredActorsForCameraPenetration(TArray<const AActor*>& OutActorsAllowPenetration) const
{
	OutActorsAllowPenetration.Add(this);
}

TOptional<AActor*> AKRHeroCharacter::GetCameraPreventPenetrationTarget() const
{
	return TOptional<AActor*>(const_cast<AKRHeroCharacter*>(this));
}
