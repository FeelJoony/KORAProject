#pragma once

#include "CoreMinimal.h"
#include "Data/DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "KORATypes/KRStructTypes.h"
#include "DataAsset_HeroStartUpData.generated.h"


UCLASS()
class KORAPROJECT_API UDataAsset_HeroStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UKRAbilitySystemComponent* InASC, int32 ApplyLevel = 1) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FKRHeroAbilitySet> HeroStartUpAbilitySets;
	
};
