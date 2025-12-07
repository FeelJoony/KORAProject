#include "PlayAbilityChecker.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

UPlayAbilityChecker::UPlayAbilityChecker()
{
}

bool UPlayAbilityChecker::CanCount(const FSubQuestEvalDataStruct& EvalData, const FGameplayTag& InTag)
{
	APlayerState* PlayerState = UGameplayStatics::GetPlayerState(GetWorld(), 0);

	if (PlayerState)
	{
		IAbilitySystemInterface* Interface = CastChecked<IAbilitySystemInterface>(PlayerState);
		UAbilitySystemComponent* ASC = Interface->GetAbilitySystemComponent();

		if (ASC->HasMatchingGameplayTag(InTag))
		{
			return true;
		}
	}
	
	return false;
}
