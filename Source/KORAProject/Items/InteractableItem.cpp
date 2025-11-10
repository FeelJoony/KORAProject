#include "InteractableItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AInteractableItem::AInteractableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(DefaultRoot);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Create interaction sphere _ 필요
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(DefaultRoot);
	InteractionSphere->SetSphereRadius(100.f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Default values _ 필요
	InteractionText = FText::FromString(TEXT("Interact"));
	InteractionSubText = FText::FromString(TEXT("Press [E] to interact"));
	bCanInteract = true;
}

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableItem::GatherInteractionOptions_Implementation(const FInteractionQuery& InteractQuery, TArray<FInteractionOption>& Options)
{
	FInteractionOption Option;
	Option.InteractableTarget = this;
	Option.Text = InteractionText;
	Option.SubText = InteractionSubText;
	Options.Add(Option);
}

void AInteractableItem::OnInteractionTriggered_Implementation(AActor* InteractingActor)
{
	// Call blueprint event
	OnInteracted(InteractingActor);

	// Default behavior: log the interaction
	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("%s interacted with %s"),
			*InteractingActor->GetName(), *GetName());
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Message);
	}
}

FVector AInteractableItem::GetInteractionIndicatorLocation_Implementation() const
{
	if (MeshComponent)
	{
		return MeshComponent->GetComponentLocation();
	}
	return GetActorLocation();
}

bool AInteractableItem::CanInteract_Implementation(const FInteractionQuery& InteractQuery) const
{
	return bCanInteract;
}
