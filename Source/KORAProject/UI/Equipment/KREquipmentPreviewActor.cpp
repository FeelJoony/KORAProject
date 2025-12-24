// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Equipment/KREquipmentPreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Animation/AnimSequence.h"
#include "Kismet/GameplayStatics.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "Inventory/KRInventoryItemInstance.h"
#include "Data/EquipDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"

AKREquipmentPreviewActor::AKREquipmentPreviewActor()
{
	PrimaryActorTick.bCanEverTick = true;

	InitializeComponents();
}

void AKREquipmentPreviewActor::InitializeComponents()
{
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
	PreviewMesh->SetupAttachment(RootScene);
	PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMesh->bReceivesDecals = false;
	PreviewMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(RootScene);
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
}

void AKREquipmentPreviewActor::BeginPlay()
{
	Super::BeginPlay();

	if (!CharacterMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = CharacterMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			PreviewMesh->SetSkeletalMesh(LoadedMesh);
		}
	}

	SetupSceneCapture();
	SpawnWeaponActors();

	EquipMessageHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		FKRUIMessageTags::EquipSlot(),
		this,
		&AKREquipmentPreviewActor::OnEquipSlotChanged
	);

	EquipUIMessageHandle = UGameplayMessageSubsystem::Get(this).RegisterListener(
		FKRUIMessageTags::EquipmentUI(),
		this,
		&AKREquipmentPreviewActor::OnEquipmentUIStateChanged
	);
}

void AKREquipmentPreviewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EquipMessageHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(EquipMessageHandle);
	}
	if (EquipUIMessageHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(EquipUIMessageHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void AKREquipmentPreviewActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsEquipmentUIOpen)
	{
		return;
	}

	if (bIsPlayingSpinAnim)
	{
		if (!PreviewMesh->IsPlaying())
		{
			OnSpinAnimFinished();
		}
	}
	else
	{
		if (!PreviewMesh->IsPlaying())
		{
			PlayRandomIdleAnimation();
		}
	}

	if (bCaptureEveryFrame && SceneCapture && SceneCapture->TextureTarget)
	{
		SceneCapture->CaptureScene();
	}
}

void AKREquipmentPreviewActor::SetupSceneCapture()
{
	if (!RenderTarget.IsNull())
	{
		UTextureRenderTarget2D* LoadedRT = RenderTarget.LoadSynchronous();
		if (LoadedRT)
		{
			SceneCapture->TextureTarget = LoadedRT;
		}
	}

	SceneCapture->ShowOnlyActors.Add(this);
}

void AKREquipmentPreviewActor::SpawnWeaponActors()
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (SwordActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		SpawnedSwordActor = World->SpawnActor<AActor>(SwordActorClass, FTransform::Identity, SpawnParams);
		if (SpawnedSwordActor)
		{
			SpawnedSwordActor->SetActorHiddenInGame(true);
			SpawnedSwordActor->AttachToComponent(PreviewMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SwordSocketName);
			SceneCapture->ShowOnlyActors.Add(SpawnedSwordActor);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PreviewActor] SpawnWeaponActors - SwordActorClass is NULL!"));
	}

	if (GunActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		SpawnedGunActor = World->SpawnActor<AActor>(GunActorClass, FTransform::Identity, SpawnParams);
		if (SpawnedGunActor)
		{
			SpawnedGunActor->SetActorHiddenInGame(true);
			SpawnedGunActor->AttachToComponent(PreviewMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, GunSocketName);
			SceneCapture->ShowOnlyActors.Add(SpawnedGunActor);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PreviewActor] SpawnWeaponActors - GunActorClass is NULL!"));
	}
}

void AKREquipmentPreviewActor::SyncWithHeroCharacter()
{
	UKREquipmentManagerComponent* HeroEquipMgr = GetHeroEquipmentManager();
	if (!HeroEquipMgr) return;

	EquippedItems.Empty();
	
	TArray<FGameplayTag> SlotTags = {
		KRTAG_ITEMTYPE_EQUIP_SWORD,
		KRTAG_ITEMTYPE_EQUIP_GUN,
		KRTAG_ITEMTYPE_EQUIP_HEAD,
		KRTAG_ITEMTYPE_EQUIP_COSTUME
	};

	for (const FGameplayTag& SlotTag : SlotTags)
	{
		if (UKRInventoryItemInstance* Inst = HeroEquipMgr->GetEquippedItemInstanceBySlotTag(SlotTag))
		{
			PreviewEquipment(Inst, false);
		}
	}
	if (HeroEquipMgr->IsCombatModeActive())
	{
		CurrentWeaponType = HeroEquipMgr->GetActiveWeaponTypeTag();
		UpdateWeaponVisibility();
	}
}

void AKREquipmentPreviewActor::PreviewEquipment(UKRInventoryItemInstance* ItemInstance, bool bPlaySpinAnim)
{
	if (!ItemInstance) return;

	const FGameplayTag& ItemTag = ItemInstance->GetItemTag();
	const FItemDataStruct* ItemData = UKRDataTablesSubsystem::Get(this).GetDataSafe<FItemDataStruct>(EGameDataType::ItemData, ItemTag);
	if (!ItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PreviewActor] PreviewEquipment - ItemData is NULL for %s"), *ItemTag.ToString());
		return;
	}

	const FEquipDataStruct* EquipData = UKRDataTablesSubsystem::Get(this).GetDataSafe<FEquipDataStruct>(EGameDataType::EquipData, static_cast<uint32>(ItemData->EquipID));
	if (!EquipData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PreviewActor] PreviewEquipment - EquipData is NULL for EquipID: %d"), ItemData->EquipID);
		return;
	}

	const FGameplayTag& SlotTag = EquipData->SlotTag;
	EquippedItems.Add(SlotTag, ItemTag);
	ApplyEquipmentVisual(SlotTag, EquipData);

	if (SlotTag == KRTAG_ITEMTYPE_EQUIP_SWORD || SlotTag == KRTAG_ITEMTYPE_EQUIP_GUN)
	{
		CurrentWeaponType = SlotTag;
		UpdateWeaponVisibility();

		if (bPlaySpinAnim)
		{
			PlaySpinThenIdle(SlotTag);
		}
		else
		{
			PlayRandomIdleAnimation();
		}
	}
}

void AKREquipmentPreviewActor::PreviewUnequip(const FGameplayTag& SlotTag)
{
	EquippedItems.Remove(SlotTag);

	if (SlotTag == KRTAG_ITEMTYPE_EQUIP_SWORD && SpawnedSwordActor)
	{
		SpawnedSwordActor->SetActorHiddenInGame(true);
		if (CurrentWeaponType == KRTAG_ITEMTYPE_EQUIP_SWORD)
		{
			CurrentWeaponType = FGameplayTag();
			PlayRandomIdleAnimation();
		}
	}
	else if (SlotTag == KRTAG_ITEMTYPE_EQUIP_GUN && SpawnedGunActor)
	{
		SpawnedGunActor->SetActorHiddenInGame(true);
		if (CurrentWeaponType == KRTAG_ITEMTYPE_EQUIP_GUN)
		{
			CurrentWeaponType = FGameplayTag();
			PlayRandomIdleAnimation();
		}
	}
}

void AKREquipmentPreviewActor::ResetPreview()
{
	SyncWithHeroCharacter();
	PlayRandomIdleAnimation();
}

void AKREquipmentPreviewActor::RotatePreview(float DeltaYaw)
{
	if (PreviewMesh)
	{
		FRotator NewRotation = PreviewMesh->GetRelativeRotation();
		NewRotation.Yaw += DeltaYaw;
		PreviewMesh->SetRelativeRotation(NewRotation);
	}
}

void AKREquipmentPreviewActor::CaptureNow()
{
	if (SceneCapture && SceneCapture->TextureTarget)
	{
		SceneCapture->CaptureScene();
	}
}

void AKREquipmentPreviewActor::PlayRandomIdleAnimation()
{
	TArray<TSoftObjectPtr<UAnimSequence>>& AnimArray = GetCurrentIdleAnimArray();
	UAnimSequence* Anim = GetRandomAnimFromArray(AnimArray);

	if (Anim)
	{
		PlayAnimation(Anim, false);
	}
}

void AKREquipmentPreviewActor::PlayAnimation(UAnimSequence* Sequence, bool bLoop)
{
	if (!PreviewMesh || !Sequence) return;

	PreviewMesh->PlayAnimation(Sequence, bLoop);
}

void AKREquipmentPreviewActor::PlaySpinThenIdle(const FGameplayTag& WeaponType)
{
	if (EquipSpinAnimation.IsNull())
	{
		PlayRandomIdleAnimation();
		return;
	}

	UAnimSequence* SpinAnim = EquipSpinAnimation.LoadSynchronous();
	if (!SpinAnim)
	{
		PlayRandomIdleAnimation();
		return;
	}

	bIsPlayingSpinAnim = true;
	PendingWeaponType = WeaponType;
	
	PlayAnimation(SpinAnim, false);
}

void AKREquipmentPreviewActor::OnSpinAnimFinished()
{
	bIsPlayingSpinAnim = false;
	CurrentWeaponType = PendingWeaponType;
	PendingWeaponType = FGameplayTag();

	UpdateWeaponVisibility();
	PlayRandomIdleAnimation();
}

UAnimSequence* AKREquipmentPreviewActor::GetRandomAnimFromArray(const TArray<TSoftObjectPtr<UAnimSequence>>& AnimArray)
{
	if (AnimArray.Num() == 0) return nullptr;

	int32 RandomIndex = FMath::RandRange(0, AnimArray.Num() - 1);
	const TSoftObjectPtr<UAnimSequence>& AnimRef = AnimArray[RandomIndex];

	if (AnimRef.IsNull()) return nullptr;

	return AnimRef.LoadSynchronous();
}

TArray<TSoftObjectPtr<UAnimSequence>>& AKREquipmentPreviewActor::GetCurrentIdleAnimArray()
{
	if (CurrentWeaponType == KRTAG_ITEMTYPE_EQUIP_SWORD && SwordIdleAnimations.Num() > 0)
	{
		return SwordIdleAnimations;
	}
	else if (CurrentWeaponType == KRTAG_ITEMTYPE_EQUIP_GUN && GunIdleAnimations.Num() > 0)
	{
		return GunIdleAnimations;
	}
	return IdleAnimations;
}

void AKREquipmentPreviewActor::ApplyEquipmentVisual(const FGameplayTag& SlotTag, const FEquipDataStruct* EquipData)
{
	if (!EquipData) return;

	if (SlotTag == KRTAG_ITEMTYPE_EQUIP_SWORD)
	{
		ApplyWeaponMesh(EquipData, true);
	}
	else if (SlotTag == KRTAG_ITEMTYPE_EQUIP_GUN)
	{
		ApplyWeaponMesh(EquipData, false);
	}
	else if (SlotTag == KRTAG_ITEMTYPE_EQUIP_COSTUME)
	{
		ApplyCostumeMaterials(EquipData);
	}
	else if (SlotTag == KRTAG_ITEMTYPE_EQUIP_HEAD)
	{
		ApplyHeadMaterial(EquipData);
	}
}

void AKREquipmentPreviewActor::ApplyWeaponMesh(const FEquipDataStruct* EquipData, bool bIsSword)
{
	AActor* TargetActor = bIsSword ? SpawnedSwordActor : SpawnedGunActor;
	if (!TargetActor)
	{
		return;
	}

	UStaticMeshComponent* MeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>();
	if (!MeshComp)
	{
		return;
	}

	if (!EquipData->EquipmentMesh.IsNull())
	{
		if (UStaticMesh* LoadedMesh = EquipData->EquipmentMesh.LoadSynchronous())
		{
			MeshComp->SetStaticMesh(LoadedMesh);
		}
	}

	for (int32 i = 0; i < EquipData->OverrideMaterials.Num(); ++i)
	{
		if (!EquipData->OverrideMaterials[i].IsNull())
		{
			if (UMaterialInterface* LoadedMat = EquipData->OverrideMaterials[i].LoadSynchronous())
			{
				MeshComp->SetMaterial(i, LoadedMat);
			}
		}
	}
}

void AKREquipmentPreviewActor::ApplyCostumeMaterials(const FEquipDataStruct* EquipData)
{
	if (!PreviewMesh || !EquipData) return;

	static FName CostumeSlotName1 = FName("MAT_CB_ARM");
	static FName CostumeSlotName2 = FName("MAT_CB_DRESS1");
	static FName CostumeSlotName3 = FName("MAT_CB_SHOES");

	int32 Index1 = PreviewMesh->GetMaterialIndex(CostumeSlotName1);
	int32 Index2 = PreviewMesh->GetMaterialIndex(CostumeSlotName2);
	int32 Index3 = PreviewMesh->GetMaterialIndex(CostumeSlotName3);

	if (EquipData->OverrideMaterials.Num() >= 3)
	{
		if (!EquipData->OverrideMaterials[0].IsNull())
		{
			PreviewMesh->SetMaterial(Index1, EquipData->OverrideMaterials[0].LoadSynchronous());
		}
		if (!EquipData->OverrideMaterials[1].IsNull())
		{
			PreviewMesh->SetMaterial(Index2, EquipData->OverrideMaterials[1].LoadSynchronous());
		}
		if (!EquipData->OverrideMaterials[2].IsNull())
		{
			PreviewMesh->SetMaterial(Index3, EquipData->OverrideMaterials[2].LoadSynchronous());
		}
	}
}

void AKREquipmentPreviewActor::ApplyHeadMaterial(const FEquipDataStruct* EquipData)
{
	if (!PreviewMesh || !EquipData) return;

	static FName HeadSlotName = FName("MAT_CB_EARS_SOCK");
	int32 HeadIndex = PreviewMesh->GetMaterialIndex(HeadSlotName);

	if (EquipData->OverrideMaterials.Num() >= 1 && !EquipData->OverrideMaterials[0].IsNull())
	{
		PreviewMesh->SetMaterial(HeadIndex, EquipData->OverrideMaterials[0].LoadSynchronous());
	}
}

void AKREquipmentPreviewActor::UpdateWeaponVisibility()
{
	if (SpawnedSwordActor)
	{
		SpawnedSwordActor->SetActorHiddenInGame(true);
	}
	if (SpawnedGunActor)
	{
		SpawnedGunActor->SetActorHiddenInGame(true);
	}

	if (CurrentWeaponType == KRTAG_ITEMTYPE_EQUIP_SWORD && SpawnedSwordActor)
	{
		SpawnedSwordActor->SetActorHiddenInGame(false);
	}
	else if (CurrentWeaponType == KRTAG_ITEMTYPE_EQUIP_GUN && SpawnedGunActor)
	{
		SpawnedGunActor->SetActorHiddenInGame(false);
	}
}

UKREquipmentManagerComponent* AKREquipmentPreviewActor::GetHeroEquipmentManager() const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return nullptr;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return nullptr;

	return Pawn->FindComponentByClass<UKREquipmentManagerComponent>();
}

void AKREquipmentPreviewActor::OnEquipSlotChanged(FGameplayTag Channel, const FKRUIMessage_EquipSlot& Msg)
{
	if (!bIsEquipmentUIOpen)
	{
		return;
	}

	if (Msg.ItemInstance)
	{
		PreviewEquipment(Msg.ItemInstance, true);
	}
	else if (Msg.SlotTag.IsValid())
	{
		PreviewUnequip(Msg.SlotTag);
	}
	CaptureNow();
}

void AKREquipmentPreviewActor::OnEquipmentUIStateChanged(FGameplayTag Channel, const FKRUIMessage_EquipmentUI& Msg)
{
	bIsEquipmentUIOpen = Msg.bIsOpen;

	if (bIsEquipmentUIOpen)
	{
		SyncWithHeroCharacter();
		PlayRandomIdleAnimation();
		CaptureNow();
	}
}
