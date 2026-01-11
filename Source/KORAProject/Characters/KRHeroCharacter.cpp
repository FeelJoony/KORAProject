#include "Characters/KRHeroCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/KRCameraComponent.h"
#include "Components/KRCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/KRHeroComponent.h"
#include "Components/KRPawnExtensionComponent.h"
#include "Components/KRStaminaComponent.h"
#include "Components/KRCoreDriveComponent.h"
#include "Components/KRGuardRegainComponent.h"
#include "MotionWarpingComponent.h"
#include "Engine/OverlapResult.h"

AKRHeroCharacter::AKRHeroCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UKRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	CameraComponent = CreateDefaultSubobject<UKRCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.f, 0.f, 75.f));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	
	HeroComponent = CreateDefaultSubobject<UKRHeroComponent>(TEXT("HeroComponent"));

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	StaminaComponent = CreateDefaultSubobject<UKRStaminaComponent>(TEXT("StaminaComponent"));

	CoreDriveComponent = CreateDefaultSubobject<UKRCoreDriveComponent>(TEXT("CoreDriveComponent"));

	GuardRegainComponent = CreateDefaultSubobject<UKRGuardRegainComponent>(TEXT("GuardRegainComponent"));

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// 포탈 시스템을 위한 플레이어 태그 추가
	Tags.Add(FName(TEXT("Player")));
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
	// 플레이어 자신 무시
	OutActorsAllowPenetration.Add(this);

	// 주변 Pawn(적) 무시 - 적의 공격 메시가 카메라 침투 감지에 걸리는 것 방지
	if (CameraPenetrationIgnoreRadius > 0.f)
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		GetWorld()->OverlapMultiByChannel(
			Overlaps,
			GetActorLocation(),
			FQuat::Identity,
			ECC_Pawn,
			FCollisionShape::MakeSphere(CameraPenetrationIgnoreRadius),
			QueryParams
		);

		for (const FOverlapResult& Overlap : Overlaps)
		{
			if (const APawn* OtherPawn = Cast<APawn>(Overlap.GetActor()))
			{
				OutActorsAllowPenetration.Add(OtherPawn);
			}
		}
	}
}

TOptional<AActor*> AKRHeroCharacter::GetCameraPreventPenetrationTarget() const
{
	return TOptional<AActor*>(const_cast<AKRHeroCharacter*>(this));
}
