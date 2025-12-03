#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "Interface/TableKey.h"
#include "EquipmentDataStruct.generated.h"

class UStreamableRenderAsset;
class UMaterialInterface;
class UTexture2D;

USTRUCT(BlueprintType)
struct KORAPROJECT_API FEquipmentDataStruct : public FTableRowBase, public ITableKey
{
	GENERATED_BODY()

	FEquipmentDataStruct() {}

public:
	//--------Identity--------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FGameplayTag ItemTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FName DisplayNameKey;
	
	//--------Visuals--------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	TSoftObjectPtr<UStreamableRenderAsset> EquipmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	TArray<TSoftObjectPtr<UMaterialInterface>> OverrideMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	TSoftObjectPtr<UTexture2D> Icon;
	
	//--------Configuration--------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer CompatibleModuleSlots;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<FGameplayTag> DefaultModuleTags;
	
public:
	virtual uint32 GetKey() const override { return GetTypeHash(ItemTag); }
};