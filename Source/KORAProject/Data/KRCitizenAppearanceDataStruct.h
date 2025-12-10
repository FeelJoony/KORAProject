// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "KRCitizenAppearanceDataStruct.generated.h"

UENUM(BlueprintType)
enum class ECitizenGroomSlot : uint8
{
    Hair,
    Eyebrows,
    Beard,
    Mustache,
    Eyelashes,
    Fuzz
};

USTRUCT(BlueprintType)
struct FCitizenGroomSlotData
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECitizenGroomSlot Slot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<class UGroomAsset> GroomAsset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<class UGroomBindingAsset> BindingAsset;

    //UPROPERTY(EditAnywhere, BlueprintReadOnly)
    //TArray<TSoftObjectPtr<UMaterialInterface>> OverrideMaterials;
};

USTRUCT(BlueprintType)
struct FKRCitizenAppearanceDataStruct : public FTableRowBase
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<USkeletalMesh> BodyMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<USkeletalMesh> ClothMesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSoftObjectPtr<USkeletalMesh> FaceMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FCitizenGroomSlotData> GroomSlots;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clavicle", meta = (EditCondition = "bOverrideClavicleOffsets"))
    FVector ClavicleRightOffset = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Clavicle", meta = (EditCondition = "bOverrideClavicleOffsets"))
    FVector ClavicleLeftOffset = FVector::ZeroVector;
};
