#include "GameModes/KRBaseGameState.h"

#include "KRExperienceManagerComponent.h"
#include "Components/GameFrameworkComponentManager.h"

AKRBaseGameState::AKRBaseGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UKRExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

void AKRBaseGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AKRBaseGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}
