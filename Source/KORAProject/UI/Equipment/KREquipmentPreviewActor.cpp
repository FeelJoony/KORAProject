// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Equipment/KREquipmentPreviewActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/AssetManager.h"
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

	PreviewMesh->CastShadow = false;
	PreviewMesh->bCastDynamicShadow = false;
	PreviewMesh->bAffectDynamicIndirectLighting = false;
	PreviewMesh->bAffectDistanceFieldLighting = false;

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(RootScene);
	SceneCapture->bCaptureEveryFrame = false;
	SceneCapture->bCaptureOnMovement = false;
	SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	
	SceneCapture->bAlwaysPersistRenderingState = true;   // 렌더링 상태 캐싱
	SceneCapture->MaxViewDistanceOverride = 500.0f;     // 뷰 거리 제한
	SceneCapture->LODDistanceFactor = 0.25f;            // LOD 적극적 사용
	SceneCapture->bUseRayTracingIfEnabled = false;      // 레이트레이싱 비활성화
	SceneCapture->DetailMode = EDetailMode::DM_Low;     // 디테일 모드 낮음
	SceneCapture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	SceneCapture->CompositeMode = ESceneCaptureCompositeMode::SCCM_Overwrite;
	
	SceneCapture->ShowFlags.SetLumenGlobalIllumination(false);
	SceneCapture->ShowFlags.SetLumenReflections(false);
	SceneCapture->ShowFlags.SetGlobalIllumination(false);
	SceneCapture->ShowFlags.SetIndirectLightingCache(false);

	SceneCapture->ShowFlags.SetRayTracedDistanceFieldShadows(false);
	SceneCapture->ShowFlags.SetReflectionEnvironment(false);
	SceneCapture->ShowFlags.SetScreenSpaceReflections(false);
	SceneCapture->ShowFlags.SetPathTracing(false);

	SceneCapture->ShowFlags.SetContactShadows(false);
	SceneCapture->ShowFlags.SetCapsuleShadows(false);

	SceneCapture->ShowFlags.SetVolumetricFog(false);
	SceneCapture->ShowFlags.SetVolumetricLightmap(false);
	SceneCapture->ShowFlags.SetFog(false);
	SceneCapture->ShowFlags.SetAtmosphere(false);
	SceneCapture->ShowFlags.SetCloud(false);
	SceneCapture->ShowFlags.SetSkyLighting(false);  // 스카이 라이팅 비활성화
	
	SceneCapture->ShowFlags.SetPostProcessing(false);
	SceneCapture->ShowFlags.SetTonemapper(false);
	SceneCapture->ShowFlags.SetToneCurve(false);
	SceneCapture->ShowFlags.SetAntiAliasing(false);    // AA 비활성화 (마젠타 halo 방지)
	SceneCapture->ShowFlags.SetTemporalAA(false);
	SceneCapture->ShowFlags.SetMotionBlur(false);
	SceneCapture->ShowFlags.SetBloom(false);
	SceneCapture->ShowFlags.SetEyeAdaptation(false);
	SceneCapture->ShowFlags.SetLocalExposure(false);
	SceneCapture->ShowFlags.SetDepthOfField(false);
	SceneCapture->ShowFlags.SetLensFlares(false);
	SceneCapture->ShowFlags.SetVignette(false);
	SceneCapture->ShowFlags.SetGrain(false);
	SceneCapture->ShowFlags.SetColorGrading(false);

	SceneCapture->ShowFlags.SetDynamicShadows(false);
	SceneCapture->ShowFlags.SetAmbientOcclusion(false);
	SceneCapture->ShowFlags.SetScreenSpaceAO(false);
	SceneCapture->ShowFlags.SetDistanceFieldAO(false);

	SceneCapture->ShowFlags.SetDecals(false);
	SceneCapture->ShowFlags.SetTranslucency(false);
	SceneCapture->ShowFlags.SetParticles(false);
	SceneCapture->ShowFlags.SetTemporalAA(false);
	SceneCapture->ShowFlags.SetHMDDistortion(false);
	
	SceneCapture->ShowFlags.SetInstancedFoliage(false);
	SceneCapture->ShowFlags.SetInstancedGrass(false);
	SceneCapture->ShowFlags.SetInstancedStaticMeshes(false);
	SceneCapture->ShowFlags.SetLandscape(false);
	SceneCapture->ShowFlags.SetBSP(false);
	SceneCapture->ShowFlags.SetPaper2DSprites(false);
	SceneCapture->ShowFlags.SetTextRender(false);
	SceneCapture->ShowFlags.SetBillboardSprites(false);
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PreviewActor] CharacterMesh is NULL! Set it in BP."));
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
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CaptureDebounceHandle);
		World->GetTimerManager().ClearTimer(EquipChangeCaptureHandle);
	}
	
	if (AnimPreloadHandle.IsValid())
	{
		AnimPreloadHandle->CancelHandle();
		AnimPreloadHandle.Reset();
	}
	if (EquipAssetPreloadHandle.IsValid())
	{
		EquipAssetPreloadHandle->CancelHandle();
		EquipAssetPreloadHandle.Reset();
	}
	CachedAnimations.Empty();
	CachedMaterials.Empty();
	CachedMeshes.Empty();

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
	if (SceneCapture && SceneCapture->TextureTarget)
	{
		CaptureTimer += DeltaTime;
		if (CaptureTimer >= CaptureInterval)
		{
			SceneCapture->CaptureScene();
			CaptureTimer = 0.0f;
		}
	}
}

void AKREquipmentPreviewActor::SetupSceneCapture()
{
	if (bCreateRuntimeRT)
	{
		RuntimeRenderTarget = NewObject<UTextureRenderTarget2D>(this, TEXT("PreviewRT"));
		if (RuntimeRenderTarget)
		{
			RuntimeRenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
			RuntimeRenderTarget->ClearColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);  // 흰색 배경 (InvOpacity용) 
			RuntimeRenderTarget->bAutoGenerateMips = false;
			RuntimeRenderTarget->bGPUSharedFlag = false;
			RuntimeRenderTarget->InitAutoFormat(RenderTargetWidth, RenderTargetHeight);
			RuntimeRenderTarget->UpdateResourceImmediate(true);

			SceneCapture->TextureTarget = RuntimeRenderTarget;
		}
	}
	else if (!RenderTarget.IsNull())
	{
		UTextureRenderTarget2D* LoadedRT = RenderTarget.LoadSynchronous();
		if (LoadedRT)
		{
			SceneCapture->TextureTarget = LoadedRT;
		}
	}

	SceneCapture->ShowOnlyActors.Add(this);
	if (PreviewMesh)
	{
		FVector MeshLocation = PreviewMesh->GetComponentLocation();
		MeshLocation.Z += 100.0f;  // 캐릭터 가슴 높이로 오프셋 (발 기준 위로 100)
		FVector CameraLocation = SceneCapture->GetComponentLocation();
		FVector DirectionToMesh = (MeshLocation - CameraLocation).GetSafeNormal();

		if (!DirectionToMesh.IsNearlyZero())
		{
			FRotator LookAtRotation = DirectionToMesh.Rotation();
			SceneCapture->SetWorldRotation(LookAtRotation);
		}
	}
	if (SceneCapture->TextureTarget)
	{
		SceneCapture->CaptureScene();
	}
}

UTextureRenderTarget2D* AKREquipmentPreviewActor::GetActiveRenderTarget() const
{
	if (RuntimeRenderTarget)
	{
		return RuntimeRenderTarget;
	}
	return SceneCapture ? SceneCapture->TextureTarget : nullptr;
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
	if (!SceneCapture || !SceneCapture->TextureTarget)
	{
		return;
	}

	if (CaptureDebounceTime > 0.0f)
	{
		bCapturePending = true;
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CaptureDebounceHandle);
			World->GetTimerManager().SetTimer(
				CaptureDebounceHandle,
				this,
				&AKREquipmentPreviewActor::ExecuteDebouncedCapture,
				CaptureDebounceTime,
				false
			);
		}
	}
	else
	{
		SceneCapture->CaptureScene();
	}
}

void AKREquipmentPreviewActor::ExecuteDebouncedCapture()
{
	if (bCapturePending && SceneCapture && SceneCapture->TextureTarget)
	{
		SceneCapture->CaptureScene();
		bCapturePending = false;
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
	UAnimSequence* SpinAnim = GetCachedOrLoadAnim(EquipSpinAnimation);
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
	return GetCachedOrLoadAnim(AnimRef);
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
		if (UStaticMesh* CachedMesh = GetCachedMesh(EquipData->EquipmentMesh))
		{
			MeshComp->SetStaticMesh(CachedMesh);
		}
		else if (UStaticMesh* LoadedMesh = EquipData->EquipmentMesh.LoadSynchronous())
		{
			MeshComp->SetStaticMesh(LoadedMesh);
		}
	}

	for (int32 i = 0; i < EquipData->OverrideMaterials.Num(); ++i)
	{
		if (!EquipData->OverrideMaterials[i].IsNull())
		{
			if (UMaterialInterface* CachedMat = GetCachedMaterial(EquipData->OverrideMaterials[i]))
			{
				MeshComp->SetMaterial(i, CachedMat);
			}
			else if (UMaterialInterface* LoadedMat = EquipData->OverrideMaterials[i].LoadSynchronous())
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
			if (UMaterialInterface* Mat = GetCachedMaterial(EquipData->OverrideMaterials[0]))
			{
				PreviewMesh->SetMaterial(Index1, Mat);
			}
			else
			{
				PreviewMesh->SetMaterial(Index1, EquipData->OverrideMaterials[0].LoadSynchronous());
			}
		}
		if (!EquipData->OverrideMaterials[1].IsNull())
		{
			if (UMaterialInterface* Mat = GetCachedMaterial(EquipData->OverrideMaterials[1]))
			{
				PreviewMesh->SetMaterial(Index2, Mat);
			}
			else
			{
				PreviewMesh->SetMaterial(Index2, EquipData->OverrideMaterials[1].LoadSynchronous());
			}
		}
		if (!EquipData->OverrideMaterials[2].IsNull())
		{
			if (UMaterialInterface* Mat = GetCachedMaterial(EquipData->OverrideMaterials[2]))
			{
				PreviewMesh->SetMaterial(Index3, Mat);
			}
			else
			{
				PreviewMesh->SetMaterial(Index3, EquipData->OverrideMaterials[2].LoadSynchronous());
			}
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
		if (UMaterialInterface* Mat = GetCachedMaterial(EquipData->OverrideMaterials[0]))
		{
			PreviewMesh->SetMaterial(HeadIndex, Mat);
		}
		else
		{
			PreviewMesh->SetMaterial(HeadIndex, EquipData->OverrideMaterials[0].LoadSynchronous());
		}
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

	if (CurrentWeaponType == KRTAG_ITEMTYPE_EQUIP_SWORD)
	{
		SpawnedSwordActor->SetActorHiddenInGame(false);
	}
	else if (CurrentWeaponType == KRTAG_ITEMTYPE_EQUIP_GUN)
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
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(EquipChangeCaptureHandle);
		World->GetTimerManager().SetTimer(
			EquipChangeCaptureHandle,
			this,
			&AKREquipmentPreviewActor::OnDelayedEquipCapture,
			0.033f,  // 약 2프레임 후 캡처 (60fps 기준)
			false
		);
	}
}

void AKREquipmentPreviewActor::OnEquipmentUIStateChanged(FGameplayTag Channel, const FKRUIMessage_EquipmentUI& Msg)
{
	bIsEquipmentUIOpen = Msg.bIsOpen;

	if (bIsEquipmentUIOpen)
	{
		bAnimationsPreloaded = false;
		bEquipAssetsPreloaded = false;
		PreloadAnimations();
		PreloadEquipmentAssets();
	}
	else
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(CaptureDebounceHandle);
			World->GetTimerManager().ClearTimer(EquipChangeCaptureHandle);
		}
		bCapturePending = false;
	}
}

void AKREquipmentPreviewActor::TryInitializeAfterPreload()
{
	if (!bAnimationsPreloaded || !bEquipAssetsPreloaded)
	{
		return;
	}

	if (!bIsEquipmentUIOpen)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[PreviewActor] All assets preloaded, initializing preview"));

	SyncWithHeroCharacter();
	PlayRandomIdleAnimation();
	ActiveCaptureRemainingTime = ActiveCaptureDuration;
	CaptureNow();
}

void AKREquipmentPreviewActor::PreloadAnimations()
{
	if (AnimPreloadHandle.IsValid() && AnimPreloadHandle->IsLoadingInProgress())
	{
		return;
	}

	TArray<FSoftObjectPath> AssetsToLoad;

	auto CollectAnims = [&AssetsToLoad](const TArray<TSoftObjectPtr<UAnimSequence>>& AnimArray)
	{
		for (const auto& Anim : AnimArray)
		{
			if (!Anim.IsNull())
			{
				AssetsToLoad.AddUnique(Anim.ToSoftObjectPath());
			}
		}
	};

	CollectAnims(IdleAnimations);
	CollectAnims(SwordIdleAnimations);
	CollectAnims(GunIdleAnimations);

	if (!EquipSpinAnimation.IsNull())
	{
		AssetsToLoad.AddUnique(EquipSpinAnimation.ToSoftObjectPath());
	}

	if (AssetsToLoad.Num() == 0)
	{
		bAnimationsPreloaded = true;
		TryInitializeAfterPreload();
		return;
	}

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	AnimPreloadHandle = StreamableManager.RequestAsyncLoad(
		AssetsToLoad,
		FStreamableDelegate::CreateUObject(this, &AKREquipmentPreviewActor::OnAnimationsPreloaded),
		FStreamableManager::AsyncLoadHighPriority
	);
}

void AKREquipmentPreviewActor::OnAnimationsPreloaded()
{
	auto CacheAnims = [this](const TArray<TSoftObjectPtr<UAnimSequence>>& AnimArray)
	{
		for (const auto& Anim : AnimArray)
		{
			if (!Anim.IsNull() && Anim.IsValid())
			{
				CachedAnimations.Add(Anim.ToSoftObjectPath(), Anim.Get());
			}
		}
	};

	CacheAnims(IdleAnimations);
	CacheAnims(SwordIdleAnimations);
	CacheAnims(GunIdleAnimations);

	if (!EquipSpinAnimation.IsNull() && EquipSpinAnimation.IsValid())
	{
		CachedAnimations.Add(EquipSpinAnimation.ToSoftObjectPath(), EquipSpinAnimation.Get());
	}

	UE_LOG(LogTemp, Log, TEXT("[PreviewActor] Animations preloaded: %d cached"), CachedAnimations.Num());

	bAnimationsPreloaded = true;
	TryInitializeAfterPreload();
}

UAnimSequence* AKREquipmentPreviewActor::GetCachedOrLoadAnim(const TSoftObjectPtr<UAnimSequence>& AnimRef)
{
	if (AnimRef.IsNull())
	{
		return nullptr;
	}

	if (TObjectPtr<UAnimSequence>* Found = CachedAnimations.Find(AnimRef.ToSoftObjectPath()))
	{
		if (*Found)
		{
			return *Found;
		}
	}
	
	UAnimSequence* Loaded = AnimRef.LoadSynchronous();
	if (Loaded)
	{
		CachedAnimations.Add(AnimRef.ToSoftObjectPath(), Loaded);
	}
	return Loaded;
}

void AKREquipmentPreviewActor::PreloadEquipmentAssets()
{
	if (EquipAssetPreloadHandle.IsValid() && EquipAssetPreloadHandle->IsLoadingInProgress())
	{
		return;
	}

	TArray<FSoftObjectPath> AssetsToLoad;

	UKREquipmentManagerComponent* HeroEquipMgr = GetHeroEquipmentManager();
	if (!HeroEquipMgr)
	{
		bEquipAssetsPreloaded = true;
		TryInitializeAfterPreload();
		return;
	}

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
			const FGameplayTag& ItemTag = Inst->GetItemTag();
			const FItemDataStruct* ItemData = UKRDataTablesSubsystem::Get(this).GetDataSafe<FItemDataStruct>(EGameDataType::ItemData, ItemTag);
			if (!ItemData) continue;

			const FEquipDataStruct* EquipData = UKRDataTablesSubsystem::Get(this).GetDataSafe<FEquipDataStruct>(EGameDataType::EquipData, static_cast<uint32>(ItemData->EquipID));
			if (!EquipData) continue;
			
			if (!EquipData->EquipmentMesh.IsNull())
			{
				AssetsToLoad.AddUnique(EquipData->EquipmentMesh.ToSoftObjectPath());
			}

			for (const TSoftObjectPtr<UMaterialInterface>& MatRef : EquipData->OverrideMaterials)
			{
				if (!MatRef.IsNull())
				{
					AssetsToLoad.AddUnique(MatRef.ToSoftObjectPath());
				}
			}
		}
	}

	if (AssetsToLoad.Num() == 0)
	{
		bEquipAssetsPreloaded = true;
		TryInitializeAfterPreload();
		return;
	}

	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	EquipAssetPreloadHandle = StreamableManager.RequestAsyncLoad(
		AssetsToLoad,
		FStreamableDelegate::CreateUObject(this, &AKREquipmentPreviewActor::OnEquipmentAssetsPreloaded),
		FStreamableManager::AsyncLoadHighPriority
	);
}

void AKREquipmentPreviewActor::OnEquipmentAssetsPreloaded()
{
	if (!EquipAssetPreloadHandle.IsValid())
	{
		return;
	}

	TArray<UObject*> LoadedAssets;
	EquipAssetPreloadHandle->GetLoadedAssets(LoadedAssets);

	for (UObject* Asset : LoadedAssets)
	{
		if (UMaterialInterface* Mat = Cast<UMaterialInterface>(Asset))
		{
			FSoftObjectPath Path(Mat);
			CachedMaterials.Add(Path, Mat);
		}
		else if (UStaticMesh* Mesh = Cast<UStaticMesh>(Asset))
		{
			FSoftObjectPath Path(Mesh);
			CachedMeshes.Add(Path, Mesh);
		}
	}
	bEquipAssetsPreloaded = true;
	TryInitializeAfterPreload();
}

UMaterialInterface* AKREquipmentPreviewActor::GetCachedMaterial(const TSoftObjectPtr<UMaterialInterface>& SoftPtr) const
{
	if (SoftPtr.IsNull())
	{
		return nullptr;
	}

	if (SoftPtr.IsValid())
	{
		return SoftPtr.Get();
	}

	if (const TObjectPtr<UMaterialInterface>* Found = CachedMaterials.Find(SoftPtr.ToSoftObjectPath()))
	{
		if (*Found)
		{
			return *Found;
		}
	}

	return nullptr;
}

UStaticMesh* AKREquipmentPreviewActor::GetCachedMesh(const TSoftObjectPtr<UStaticMesh>& SoftPtr) const
{
	if (SoftPtr.IsNull())
	{
		return nullptr;
	}

	if (SoftPtr.IsValid())
	{
		return SoftPtr.Get();
	}

	if (const TObjectPtr<UStaticMesh>* Found = CachedMeshes.Find(SoftPtr.ToSoftObjectPath()))
	{
		if (*Found)
		{
			return *Found;
		}
	}

	return nullptr;
}

void AKREquipmentPreviewActor::OnDelayedEquipCapture()
{
	if (!bIsEquipmentUIOpen)
	{
		return;
	}

	CaptureNow();
}
