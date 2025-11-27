#include "GameModes/KRBaseGameState.h"

#include "KRExperienceManagerComponent.h"

AKRBaseGameState::AKRBaseGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UKRExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
