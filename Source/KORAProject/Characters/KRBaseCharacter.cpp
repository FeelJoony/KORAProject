#include "Characters/KRBaseCharacter.h"

#include "Data/DataAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Player/KRPlayerState.h"

AKRBaseCharacter::AKRBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetMesh()->bReceivesDecals = false;
}

UAbilitySystemComponent* AKRBaseCharacter::GetAbilitySystemComponent() const
{
	return CachedASC;
}

UPawnCombatComponent* AKRBaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

void AKRBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AKRPlayerState* KRPS = GetPlayerState<AKRPlayerState>())
	{
		KRPS->InitASCForAvatar(this);
		SetCachedASC(KRPS->GetAbilitySystemComponent());

		ensureMsgf(!CharacterStartUpData.IsNull(), TEXT("Forgot to assing start up data to %s"), *GetName());
	}
}
