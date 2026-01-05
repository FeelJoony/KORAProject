#include "Interaction/KRQuestItemPickup.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Subsystem/KRInventorySubsystem.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

AKRQuestItemPickup::AKRQuestItemPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionProfileName(TEXT("NoCollision"));

	InteractCollision->SetupAttachment(ItemMesh);
}

void AKRQuestItemPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AKRQuestItemPickup::InitializeQuestItem(FGameplayTag InItemTag)
{
	QuestItemTag = InItemTag;
}

void AKRQuestItemPickup::PickupItem()
{
	if (bIsPickedUp || !QuestItemTag.IsValid())
	{
		return;
	}

	bIsPickedUp = true;

	UGameInstance* GI = GetWorld()->GetGameInstance();
	if (!GI)
	{
		Destroy();
		return;
	}

	UKRInventorySubsystem* InvSystem = GI->GetSubsystem<UKRInventorySubsystem>();
	if (InvSystem)
	{
		InvSystem->AddItem(QuestItemTag, 1);

		FKRUIMessage_ItemLog Payload;
		Payload.EventType = EItemLogEvent::AcquiredQuest;
		Payload.ItemTag = QuestItemTag;
		Payload.NewTotalQuantity = InvSystem->GetItemCountByTag(QuestItemTag);

		UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(FKRUIMessageTags::ItemLog(), Payload);
	}

	FAddItemMessage QuestItemMessage;
	QuestItemMessage.ItemTag = QuestItemTag;
	QuestItemMessage.StackCount = 1;
	UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(QuestItemTag, QuestItemMessage);

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(0.5f);
}
