#include "Characters/Citizen/KRCitizenCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GroomComponent.h"

#include "Animation/AnimInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/UnrealType.h"
#include "Math/UnrealMathUtility.h"

AKRCitizenCharacter::AKRCitizenCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(35.f, 96.f);
	RootComponent = CapsuleComponent;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetupAttachment(CapsuleComponent);
	Body = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(Root);
	Body->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
	CitizenClothes = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CitizenClothes"));
	CitizenClothes->SetupAttachment(Body);

	Face = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face"));
	Face->SetupAttachment(Body);
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(Face);
	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(Face);
	Beard = CreateDefaultSubobject<UGroomComponent>(TEXT("Beard"));
	Beard->SetupAttachment(Face);
	Mustache = CreateDefaultSubobject<UGroomComponent>(TEXT("Mustache"));
	Mustache->SetupAttachment(Face);
	Eyelashes = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyelashes"));
	Eyelashes->SetupAttachment(Face);
	Fuzz = CreateDefaultSubobject<UGroomComponent>(TEXT("Fuzz"));
	Fuzz->SetupAttachment(Face);
}

UGroomComponent* AKRCitizenCharacter::GetGroomComponent(ECitizenGroomSlot Slot) const
{
	switch (Slot)
	{
	case ECitizenGroomSlot::Hair:		return Hair;
	case ECitizenGroomSlot::Eyebrows:	return Eyebrows;
	case ECitizenGroomSlot::Beard:		return Beard;
	case ECitizenGroomSlot::Mustache:	return Mustache;
	case ECitizenGroomSlot::Eyelashes:	return Eyelashes;
	case ECitizenGroomSlot::Fuzz:		return Fuzz;
	default:							return nullptr;
	}
}

void AKRCitizenCharacter::ApplyAppearanceFromRow(const FKRCitizenAppearanceData& Row)
{
	if (Body && Row.BodyMesh.IsValid())
	{
		if (USkeletalMesh* BodyMesh = Row.BodyMesh.Get()) // LoadSynchronous()
		{
			Body->SetSkeletalMesh(BodyMesh);
		}
	}

	if (CitizenClothes && Row.ClothMesh.IsValid())
	{
		if (USkeletalMesh* ClothMesh = Row.ClothMesh.Get())
		{
			CitizenClothes->SetSkeletalMesh(ClothMesh);
		}
	}

	if (Face && Row.FaceMesh.IsValid())
	{
		if (USkeletalMesh* FaceMesh = Row.FaceMesh.Get())
		{
			Face->SetSkeletalMesh(FaceMesh);
		}
	}

	ApplyGroomSlots(Row);

	ClavicleRightOffset = Row.ClavicleRightOffset;
	ClavicleLeftOffset = Row.ClavicleLeftOffset;
	if (Body)
	{
		ApplyClavicleOffsets(Body);
	}

	if (CitizenClothes)
	{
		RandomizeClothingColors(CitizenClothes);
	}
}

void AKRCitizenCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (MHAppearanceTable && !AppearanceCharacterRowID.IsNone())
	{
		if (const FKRCitizenAppearanceData* Row = MHAppearanceTable->FindRow<FKRCitizenAppearanceData>(AppearanceCharacterRowID, TEXT("KRCitizen (BeginPlay)")))
		{
			ApplyAppearanceFromRow(*Row);
		}
	}

	if (CitizenClothes && CachedClothingColors.Num() == 0) RandomizeClothingColors(CitizenClothes);
}


void AKRCitizenCharacter::RandomizeClothingColors(USkeletalMeshComponent* TargetClothMesh)
{
	if (!TargetClothMesh) return;
	if (ColorParameterNames.Num() == 0) return;

	TArray<FLinearColor> Colors;
	Colors.Reserve(ColorParameterNames.Num());

	for (int32 i = 0; i < ColorParameterNames.Num(); ++i)
	{
		Colors.Add(MakeRandomColor());
	}
	CachedClothingColors = Colors;
	ApplyRandomClothingColors(TargetClothMesh, Colors);
}

FLinearColor AKRCitizenCharacter::MakeRandomColor() const
{
	auto RandChannel = [&](float Min, float Max)
		{
			return FMath::FRandRange(Min, Max);
		};

	FLinearColor Result;
	Result.R = RandChannel(RandomColorMin.R, RandomColorMax.R);
	Result.G = RandChannel(RandomColorMin.G, RandomColorMax.G);
	Result.B = RandChannel(RandomColorMin.B, RandomColorMax.B);
	Result.A = 1.0f;

	return Result;
}

void AKRCitizenCharacter::ApplyRandomClothingColors(USkeletalMeshComponent* TargetClothMesh, const TArray<FLinearColor>& Colors)
{
	if (!TargetClothMesh) return;
	if (ColorParameterNames.Num() == 0) return;
	if (Colors.Num() == 0) return;

	int32 ColorIndex = 0;
	const int32 MatCount = TargetClothMesh->GetNumMaterials();
	const int32 ParamCount = ColorParameterNames.Num();

	for (int32 MatIndex = 0; MatIndex < MatCount; ++MatIndex)
	{
		UMaterialInterface* BaseMat = TargetClothMesh->GetMaterial(MatIndex);

		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(BaseMat);
		if (!MID)
		{
			MID = TargetClothMesh->CreateAndSetMaterialInstanceDynamic(MatIndex);
		}
		if (!MID) continue;

		for (int32 ParamIdx = 0; ParamIdx < ParamCount; ++ParamIdx)
		{
			const FName& ParamName = ColorParameterNames[ParamIdx];
			const FLinearColor& ChosenColor = Colors[ColorIndex];

			MID->SetVectorParameterValue(ParamName, ChosenColor);
			ColorIndex = (ColorIndex + 1) % Colors.Num();
		}
	}
}

void AKRCitizenCharacter::ApplyClavicleOffsets(USkeletalMeshComponent* BodyMesh)
{
	if (!BodyMesh) return;

	UAnimInstance* AnimInstance = BodyMesh->GetAnimInstance();
	if (!AnimInstance) return;

	SetAnimInstanceVectorProperty(AnimInstance, TEXT("clavicle_r_Translation"), ClavicleRightOffset);
	SetAnimInstanceVectorProperty(AnimInstance, TEXT("clavicle_l_Translation"), ClavicleLeftOffset);
}

void AKRCitizenCharacter::SetAnimInstanceVectorProperty(UAnimInstance* AnimInstance, const FName& PropertyName, const FVector& Value)
{
	if (!AnimInstance) return;

	if (FStructProperty* StructProp = FindFProperty<FStructProperty>(AnimInstance->GetClass(), PropertyName))
	{
		if (StructProp->Struct == TBaseStructure<FVector>::Get())
		{
			void* PropPtr = StructProp->ContainerPtrToValuePtr<void>(AnimInstance);
			if (PropPtr)
			{
				FVector* VectorPtr = reinterpret_cast<FVector*>(PropPtr);
				*VectorPtr = Value;
			}
		}
	}
}

void AKRCitizenCharacter::ApplyGroomSlots(const FKRCitizenAppearanceData& Row)
{
	for (const FCitizenGroomSlotData& SlotData : Row.GroomSlots)
	{
		UGroomComponent* GroomComp = GetGroomComponent(SlotData.Slot);
		if (!GroomComp) continue;

		if (SlotData.GroomAsset.IsValid())
		{
			if (UGroomAsset* Groom = SlotData.GroomAsset.LoadSynchronous())
			{
				GroomComp->SetGroomAsset(Groom);
			}
		}

		if (SlotData.BindingAsset.IsValid())
		{
			if (UGroomBindingAsset* Binding = SlotData.BindingAsset.LoadSynchronous())
			{
				GroomComp->SetBindingAsset(Binding);
			}
		}
	}
}