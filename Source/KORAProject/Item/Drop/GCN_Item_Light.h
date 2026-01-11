#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GCN_Item_Light.generated.h"

class UPointLightComponent;

UCLASS()
class KORAPROJECT_API AGCN_Item_Light : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	AGCN_Item_Light();
	
	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Light")
	TObjectPtr<UPointLightComponent> LightComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Light")
	FName AttachSocketName = "LightSocket";
};
