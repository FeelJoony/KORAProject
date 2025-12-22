#include "Interaction/KRChestActor.h"
#include "Interaction/KRDropItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "GameplayTag/KRAbilityTag.h"
#include "GameplayTag/KRShopTag.h"

using namespace std;

AKRChestActor::AKRChestActor()
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);
	
	BlockCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockCollision"));
	BlockCollision->SetupAttachment(DefaultSceneRoot);
	BlockCollision->SetBoxExtent(FVector(40.f, 80.f, 40.f));
	BlockCollision->SetCollisionProfileName(TEXT("BlockAll"));
	BlockCollision->SetRelativeLocation(FVector(0.f, 0.f, 40.f));
	
	ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMesh"));
	ChestMesh->SetupAttachment(DefaultSceneRoot);
	ChestMesh->SetCollisionProfileName(TEXT("NoCollision"));
	
	if (InteractCollision)
	{
		InteractCollision->SetupAttachment(DefaultSceneRoot);
		InteractCollision->SetRelativeLocation(FVector(0.f, 50.f, 0.f));
	}

	InteractAbilityTag = KRTAG_ABILITY_INTERACT;
	bIsOpened = false;
}

void AKRChestActor::BeginPlay()
{
	Super::BeginPlay();
}

void AKRChestActor::OnAbilityActivated(UGameplayAbility* Ability)
{
	Super::OnAbilityActivated(Ability);

	if (bIsOpened) return;
	if (!Ability || !Ability->AbilityTags.HasTag(InteractAbilityTag)) return;

	AActor* Interactor = Ability->GetAvatarActorFromActorInfo();
	if (!Interactor) return;

	OpenChest(Interactor);
}

void AKRChestActor::OpenChest(AActor* Interactor)
{
	if (bIsOpened) return;
	bIsOpened = true;
	
	if (ChestMesh && ChestOpenAnim)
	{
		ChestMesh->PlayAnimation(ChestOpenAnim, false);
	}
	
	if (OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
	}

	FGameplayEventData Payload;
	Payload.Instigator = this;
	Payload.Target = this;
	Payload.EventTag = KRTAG_ABILITY_BOXOPEN; 

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Interactor, KRTAG_ABILITY_BOXOPEN, Payload);

	FTimerHandle DropHandle;
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &AKRChestActor::SpawnLoot, Interactor);
	GetWorld()->GetTimerManager().SetTimer(DropHandle, TimerDel, 0.5f, false);

	RemoveTagFromActor(Interactor, InteractAbilityTag);
	HideInteractionUI();
}

FGameplayTag AKRChestActor::GetRandomItemTag(int32& OutQuantity)
{
	float RandVal = FMath::FRandRange(0.f, 100.f);
	OutQuantity = 1;

	if (RandVal < 35.f) return KRTAG_ITEMTYPE_CONSUME_POTION_FIRSTAID;
	RandVal -= 35.f;

	if (RandVal < 35.f) return KRTAG_ITEMTYPE_CONSUME_POTION_STAMINA;
	RandVal -= 35.f;

	if (RandVal < 8.f) return KRTAG_ITEMTYPE_CONSUME_POTION_VIOLICALL;
	RandVal -= 8.f;

	if (RandVal < 7.f) return KRTAG_ITEMTYPE_CONSUME_POTION_ATKBOOST;
	RandVal -= 7.f;

	if (RandVal < 5.f) return KRTAG_ITEMTYPE_CONSUME_POTION_SUPPRESS;
	RandVal -= 5.f;
	
	if (RandVal < 3.f) return KRTAG_ITEMTYPE_CONSUME_POTION_COREBATTERY;

	OutQuantity = FMath::RandRange(30, 120);
	return KRTAG_CURRENCY_PURCHASE_GEARING; 
}

void AKRChestActor::SpawnLoot(AActor* Interactor)
{
	if (!DropItemClass) return;

	int32 DropCount = FMath::RandRange(MinDropCount, MaxDropCount);

	for (int32 i = 0; i < DropCount; ++i)
	{
		int32 Quantity = 1;
		FGameplayTag ItemTag = GetRandomItemTag(Quantity);

		FVector SpawnLoc = GetActorLocation() + FVector(0.f, 0.f, 80.f); 
		FVector RandomOffset = FMath::VRand();
		RandomOffset.Z = FMath::Abs(RandomOffset.Z); 
		RandomOffset *= FMath::RandRange(20.f, 50.f);
		SpawnLoc += RandomOffset;

		FRotator SpawnRot = FRotator::ZeroRotator;

		AKRDropItem* DropItem = GetWorld()->SpawnActor<AKRDropItem>(DropItemClass, SpawnLoc, SpawnRot);
		if (DropItem)
		{
			DropItem->InitializeDrop(ItemTag, Quantity, Interactor);
		}
	}
}