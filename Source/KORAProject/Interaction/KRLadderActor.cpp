#include "Interaction/KRLadderActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/SphereComponent.h"
#include "GameplayTag/KRAbilityTag.h"

AKRLadderActor::AKRLadderActor()
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);
	
	LadderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->SetupAttachment(DefaultSceneRoot);
	LadderMesh->SetCollisionProfileName(TEXT("NoCollision"));
	
	if (InteractCollision)
	{
		InteractCollision->SetupAttachment(DefaultSceneRoot);
		InteractCollision->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	}
	
	TopPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TopPoint"));
	TopPoint->SetupAttachment(DefaultSceneRoot);
	TopPoint->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

	BottomPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BottomPoint"));
	BottomPoint->SetupAttachment(DefaultSceneRoot);
	BottomPoint->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

	LadderYawOffset = 0.f;
	InteractAbilityTag = KRTAG_ABILITY_INTERACT;
}

void AKRLadderActor::BeginPlay()
{
	Super::BeginPlay();
}

FTransform AKRLadderActor::GetSnapTransform(const FVector& InActorLocation) const
{
	FVector BaseForward = GetActorForwardVector();
	FVector CorrectedForward = BaseForward.RotateAngleAxis(LadderYawOffset, FVector::UpVector);

	FRotator TargetRotation = CorrectedForward.Rotation();
	TargetRotation.Yaw += 180.f;

	FVector Top = GetTopLocation();
	FVector Bottom = GetBottomLocation();
	FVector ClosestPoint = FMath::ClosestPointOnSegment(InActorLocation, Bottom, Top);
	
	const float OffsetDistance = 65.f; 
	FVector TargetLocation = ClosestPoint + (CorrectedForward * OffsetDistance);

	return FTransform(TargetRotation, TargetLocation);
}

FVector AKRLadderActor::GetTopLocation() const
{
	return TopPoint->GetComponentLocation();
}

FVector AKRLadderActor::GetBottomLocation() const
{
	return BottomPoint->GetComponentLocation();
}

void AKRLadderActor::OnAbilityActivated(UGameplayAbility* InAbility)
{
	Super::OnAbilityActivated(InAbility);

	if (!InAbility || !InAbility->GetAssetTags().HasTag(KRTAG_ABILITY_INTERACT)) { UE_LOG(LogTemp, Warning, TEXT("[Ladder] Ability Tag Mismatch. Needed: Ability.Interact"));
		return; }

	if (!ObservedASC) { return; }

	FGameplayEventData PayLoad;
	PayLoad.Instigator = this;
	PayLoad.Target = this;
	PayLoad.EventTag = KRTAG_ABILITY_LADDER;
	PayLoad.EventMagnitude = 1.f;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ObservedASC->GetAvatarActor(),KRTAG_ABILITY_LADDER, PayLoad);
}

