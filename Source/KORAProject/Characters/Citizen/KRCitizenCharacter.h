// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Data/KRCitizenAppearanceDataStruct.h"
#include "KRCitizenCharacter.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;
class USceneComponent;
class UGroomComponent;
class UAnimInstance;

UCLASS()
class KORAPROJECT_API AKRCitizenCharacter : public AActor
{
	GENERATED_BODY()
	
public:
	AKRCitizenCharacter();

	UGroomComponent* GetGroomComponent(ECitizenGroomSlot Slot) const;
	void ApplyAppearanceFromRow(const FKRCitizenAppearanceDataStruct& Row);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	USkeletalMeshComponent* Body;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	USkeletalMeshComponent* CitizenClothes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	USkeletalMeshComponent* Face;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	UGroomComponent* Hair;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	UGroomComponent* Eyebrows;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	UGroomComponent* Beard;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	UGroomComponent* Mustache;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	UGroomComponent* Eyelashes;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Components")
	UGroomComponent* Fuzz;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Citizen|Appearance")
	UDataTable* MHAppearanceTable = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Citizen|Appearance")
	FName AppearanceCharacterRowID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Citizen|Clothing")
	FLinearColor RandomColorMin = FLinearColor(0.05f, 0.05f, 0.05f, 1.f);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Citizen|Clothing")
	FLinearColor RandomColorMax = FLinearColor(0.35f, 0.35f, 0.35f, 1.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Citizen|Clothing")
	TArray<FName> ColorParameterNames = { TEXT("Color1"), TEXT("Color2"), TEXT("Color3"), TEXT("Color4") };
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Citizen|Clothing")
	TArray<FLinearColor> CachedClothingColors;

	UPROPERTY(EditAnywhere, Category = "Citizen|Clavicle") FVector ClavicleRightOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, Category = "Citizen|Clavicle") FVector ClavicleLeftOffset = FVector::ZeroVector;

private:
	void RandomizeClothingColors(USkeletalMeshComponent* TargetClothMesh);
	FLinearColor MakeRandomColor() const;
	void ApplyRandomClothingColors(USkeletalMeshComponent* TargetClothMesh, const TArray<FLinearColor>& Colors);

	void ApplyClavicleOffsets(USkeletalMeshComponent* BodyMesh);
	void SetAnimInstanceVectorProperty(UAnimInstance* AnimInstance, const FName& PropertyName, const FVector& Value);

	void ApplyGroomSlots(const FKRCitizenAppearanceDataStruct& Row);
};
