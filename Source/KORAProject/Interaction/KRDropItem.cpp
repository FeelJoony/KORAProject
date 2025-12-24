#include "Interaction/KRDropItem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerController.h"
#include "Subsystem/KRInventorySubsystem.h"
#include "Components/KRCurrencyComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "GameplayTag/KRShopTag.h"

AKRDropItem::AKRDropItem()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphere"));
	RootSphere->SetSphereRadius(20.f);
	RootSphere->SetCollisionProfileName(TEXT("NoCollision"));
	SetRootComponent(RootSphere);

	ItemVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemVFX"));
	ItemVFXComponent->SetupAttachment(RootSphere);
	ItemVFXComponent->SetAutoActivate(false);

	bCanPickup = false;
	bIsConsumed = false;
	Quantity = 1;
}

void AKRDropItem::BeginPlay()
{
	Super::BeginPlay();
}

void AKRDropItem::InitializeDrop(FGameplayTag InItemTag, int32 InQuantity, AActor* InTargetActor)
{
	ItemTag = InItemTag;
	Quantity = InQuantity;
	TargetPlayer = InTargetActor;
	
	UNiagaraSystem* SelectedSystem = DefaultVFX;
	if (RarityVFXMap.Contains(ItemTag))
	{
		SelectedSystem = RarityVFXMap[ItemTag];
	}

	if (SelectedSystem)
	{
		ItemVFXComponent->SetAsset(SelectedSystem);
		ItemVFXComponent->Activate(true);
	}
	
	FTimerHandle WaitHandle;
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, this, &AKRDropItem::EnablePickup, PickupDelay, false);
}

void AKRDropItem::EnablePickup()
{
	bCanPickup = true;
	SetActorTickEnabled(true);
}

void AKRDropItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bCanPickup || bIsConsumed || !TargetPlayer.IsValid()) return;

	FVector MyLoc = GetActorLocation();
	FVector TargetLoc = TargetPlayer->GetActorLocation();
	
	TargetLoc.Z += 50.0f; 

	FVector NewLoc = FMath::VInterpConstantTo(MyLoc, TargetLoc, DeltaTime, FlySpeed);
	SetActorLocation(NewLoc);
	
	float DistSq = FVector::DistSquared(NewLoc, TargetLoc);
	if (DistSq < (PickupRange * PickupRange))
	{
		GiveItemToPlayer();
	}
}

void AKRDropItem::GiveItemToPlayer()
{
	if (bIsConsumed) return;
	bIsConsumed = true;

	if (!TargetPlayer.IsValid())
	{
		Destroy();
		return;
	}

	UGameInstance* GI = GetWorld()->GetGameInstance();
	if (!GI) return;
	
	bool bIsCurrency = (ItemTag == KRTAG_CURRENCY_PURCHASE_GEARING) || ItemTag.MatchesTag(KRTAG_CURRENCY_PURCHASE_GEARING);

	if (bIsCurrency)
	{
		if (UKRCurrencyComponent* CurrencyComp = TargetPlayer->FindComponentByClass<UKRCurrencyComponent>())
		{
			CurrencyComp->AddCurrency(KRTAG_CURRENCY_PURCHASE_GEARING, Quantity);
		}
	}
	else
	{
		if (UKRInventorySubsystem* InvSystem = GI->GetSubsystem<UKRInventorySubsystem>())
		{
			InvSystem->AddItem(ItemTag, Quantity);
			
			FKRUIMessage_ItemLog Payload;
			Payload.EventType = EItemLogEvent::AcquiredNormal;
			Payload.ItemTag = ItemTag;
			Payload.NewTotalQuantity = InvSystem->GetItemCountByTag(ItemTag);
			
			if (const UKRInventoryItemDefinition* Def = InvSystem->GetItemDefinition(ItemTag))
			{
				UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(FKRUIMessageTags::ItemLog(), Payload);
			}
		}
	}

	SpawnPickupVFX();
	Destroy();
}

void AKRDropItem::SpawnPickupVFX()
{
	if (PickupSuccessVFX && TargetPlayer.IsValid())
	{
		UNiagaraComponent* VFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
			PickupSuccessVFX,
			TargetPlayer->GetRootComponent(),
			NAME_None,
			FVector(0.f, 0.f, 50.f),
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
}