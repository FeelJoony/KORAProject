#include "GCN_Item_Light.h"

#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"

AGCN_Item_Light::AGCN_Item_Light()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bAutoDestroyOnRemove = true;
	
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;
	
	LightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	LightComp->SetupAttachment(RootComponent);
}

static USceneComponent* GetAttachRoot(AActor* Target)
{
	if (!Target) return nullptr;
	
	if (UCapsuleComponent* Capsule = Target->FindComponentByClass<UCapsuleComponent>())
	{
		return Capsule;
	}

	return Target->GetRootComponent();
}

bool AGCN_Item_Light::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (!MyTarget) return false;
	
	if (LightComp)
	{
		LightComp->SetVisibility(true);
		LightComp->Activate(true);
		LightComp->SetRelativeLocation(FVector(0, 0, 180));
	}
	
	if (USceneComponent* AttachRoot = GetAttachRoot(MyTarget))
	{
		FAttachmentTransformRules Rules(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			EAttachmentRule::KeepRelative,
			false
		);

		AttachToComponent(AttachRoot, Rules, AttachSocketName);
	}

	return true;
}

bool AGCN_Item_Light::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if (LightComp)
	{
		LightComp->Deactivate();
		LightComp->SetVisibility(false);
	}

	return true;
}