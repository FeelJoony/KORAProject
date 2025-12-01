#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KRPawnData.generated.h"

class UKRInventoryItemDefinition;
class UKRWeaponDefinition;
class UKRCameraMode;
class UKRAbilitySet;
class UDataAsset_InputConfig;
class UKRAbilityTagRelationshipMapping;

UCLASS(BlueprintType, Const)
class KORAPROJECT_API UKRPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UKRPawnData(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Abilities")
	TArray<TObjectPtr<UKRAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Abilities")
	TObjectPtr<UKRAbilityTagRelationshipMapping> TagRelationshipMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Input")
	TObjectPtr<UDataAsset_InputConfig> InputConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Camera")
	TSubclassOf<UKRCameraMode> DefaultCameraMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Inventory")
	TArray<TObjectPtr<UKRWeaponDefinition>> DefaultEquipWeapons;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KR|Inventory")
	TArray<TSubclassOf<UKRInventoryItemDefinition>> DefaultItems;
};
