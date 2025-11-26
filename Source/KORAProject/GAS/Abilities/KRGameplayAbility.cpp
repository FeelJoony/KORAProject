#include "GAS/Abilities/KRGameplayAbility.h"

#include "Characters/KRBaseCharacter.h"
#include "Components/Combat/PawnCombatComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Player/KRPlayerController.h"

UKRGameplayAbility::UKRGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EKRAbilityActivationPolicy::OnInputTriggered;
}

UKRAbilitySystemComponent* UKRGameplayAbility::GetKRAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UKRAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AKRPlayerController* UKRGameplayAbility::GetKRPlayerControllerFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AKRPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* UKRGameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}
	}
	return nullptr;
}

AKRBaseCharacter* UKRGameplayAbility::GetKRCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AKRBaseCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UPawnCombatComponent* UKRGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

void UKRGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec) const
{
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EKRAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UKRGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	
}

void UKRGameplayAbility::OnPawnAvatarSet()
{
	K2_OnPawnAvatarSet();
}