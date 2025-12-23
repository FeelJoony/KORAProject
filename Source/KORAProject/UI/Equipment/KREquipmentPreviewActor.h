// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "KREquipmentPreviewActor.generated.h"

class USkeletalMeshComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class UKRInventoryItemInstance;
class UAnimSequence;
struct FEquipDataStruct;
struct FKRUIMessage_EquipSlot;
struct FKRUIMessage_EquipmentUI;

UCLASS(Blueprintable)
class KORAPROJECT_API AKREquipmentPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	AKREquipmentPreviewActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Mesh")
	TSoftObjectPtr<USkeletalMesh> CharacterMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Animation")
	TArray<TSoftObjectPtr<UAnimSequence>> IdleAnimations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Animation")
	TSoftObjectPtr<UAnimSequence> EquipSpinAnimation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Animation")
	TArray<TSoftObjectPtr<UAnimSequence>> SwordIdleAnimations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Animation")
	TArray<TSoftObjectPtr<UAnimSequence>> GunIdleAnimations;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Animation")
	float AnimBlendTime = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Capture")
	TSoftObjectPtr<UTextureRenderTarget2D> RenderTarget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Capture")
	bool bCaptureEveryFrame = true;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Weapon")
	TSubclassOf<AActor> SwordActorClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Weapon")
	TSubclassOf<AActor> GunActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Weapon")
	FName SwordSocketName = FName("hand_rSocket");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Preview|Weapon")
	FName GunSocketName = FName("hand_lSocket");


	UFUNCTION(BlueprintCallable, Category = "Preview")
	void SyncWithHeroCharacter();

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void PreviewEquipment(UKRInventoryItemInstance* ItemInstance, bool bPlaySpinAnim = true);

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void PreviewUnequip(const FGameplayTag& SlotTag);

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void ResetPreview();

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void RotatePreview(float DeltaYaw);

	UFUNCTION(BlueprintCallable, Category = "Preview")
	void CaptureNow();

	UFUNCTION(BlueprintPure, Category = "Preview")
	FGameplayTag GetEquippedWeaponType() const { return CurrentWeaponType; }

	UFUNCTION(BlueprintCallable, Category = "Preview|Animation")
	void PlayRandomIdleAnimation();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> PreviewMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;

	UPROPERTY()
	TObjectPtr<AActor> SpawnedSwordActor;

	UPROPERTY()
	TObjectPtr<AActor> SpawnedGunActor;

	UPROPERTY()
	FGameplayTag CurrentWeaponType;

	UPROPERTY()
	TMap<FGameplayTag, FGameplayTag> EquippedItems; // SlotTag -> ItemTag

	bool bIsPlayingSpinAnim = false;

	FGameplayTag PendingWeaponType;
	FGameplayMessageListenerHandle EquipMessageHandle;
	FGameplayMessageListenerHandle EquipUIMessageHandle;
	bool bIsEquipmentUIOpen = false;

private:
	void InitializeComponents();
	void SetupSceneCapture();
	void SpawnWeaponActors();

	void ApplyEquipmentVisual(const FGameplayTag& SlotTag, const FEquipDataStruct* EquipData);
	void ApplyWeaponMesh(const FEquipDataStruct* EquipData, bool bIsSword);
	void ApplyCostumeMaterials(const FEquipDataStruct* EquipData);
	void ApplyHeadMaterial(const FEquipDataStruct* EquipData);

	void UpdateWeaponVisibility();

	void PlayAnimation(UAnimSequence* Sequence, bool bLoop = true);
	void PlaySpinThenIdle(const FGameplayTag& WeaponType);
	UAnimSequence* GetRandomAnimFromArray(const TArray<TSoftObjectPtr<UAnimSequence>>& AnimArray);
	TArray<TSoftObjectPtr<UAnimSequence>>& GetCurrentIdleAnimArray();

	void OnSpinAnimFinished();

	class UKREquipmentManagerComponent* GetHeroEquipmentManager() const;

	void OnEquipSlotChanged(FGameplayTag Channel, const FKRUIMessage_EquipSlot& Msg);
	void OnEquipmentUIStateChanged(FGameplayTag Channel, const FKRUIMessage_EquipmentUI& Msg);
};
