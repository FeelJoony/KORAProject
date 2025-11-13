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

	// Create interaction sphere _ 필요
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(DefaultRoot);
	InteractionSphere->SetSphereRadius(100.f);

	// Default values _ (Editor에서 설정 가능) (생략가능)
	InteractionText = FText::FromString(TEXT("Interact"));
	InteractionSubText = FText::FromString(TEXT("Press [E] to interact"));
}

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableItem::GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& OptionBuilder)
{
	FInteractionOption Option;
	Option.InteractableTarget = this;
	Option.Priority= Priority; //상호작용 우선순위
	Option.Text = InteractionText; //상호작용 이름
	Option.SubText = InteractionSubText; //상호작용 설명
	Option.InteractionAbilityToGrant = InteractionGA; //Player 실행 GA
	OptionBuilder.AddInteractionOption(Option);
}

//키 입력 시 실행
void AInteractableItem::CustomizeInteractionEventData(const FGameplayTag& InteractionEventTag,
	FGameplayEventData& InOutEventData)
{
	UE_LOG(LogTemp,Warning,TEXT("[InteractableItem] CustomizeInteractionEventData"));
}
