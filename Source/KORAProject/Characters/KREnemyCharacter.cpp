#include "Characters/KREnemyCharacter.h"
#include "Components/KRPawnExtensionComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KREnemyAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTag/KREnemyTag.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KREventTag.h"
#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Stun.h"
#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Alert.h"

#include "AIController.h"
#include "Components/StateTreeComponent.h"
#include "KORADebugHelper.h"

AKREnemyCharacter::AKREnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;
	
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	
	EnemyASC = CreateDefaultSubobject<UKRAbilitySystemComponent>(TEXT("EnemyASC"));
	EnemyASC->SetIsReplicated(true);
	EnemyASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CombatCommonSet = CreateDefaultSubobject<UKRCombatCommonSet>(TEXT("CombatCommonSet"));
	EnemyAttributeSet = CreateDefaultSubobject<UKREnemyAttributeSet>(TEXT("EnemyAttributeSet"));

	SetNetUpdateFrequency(100.0f);
}

UAbilitySystemComponent* AKREnemyCharacter::GetAbilitySystemComponent() const
{
	return EnemyASC;
}

void AKREnemyCharacter::SetPatrolSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
}

void AKREnemyCharacter::SetAlertSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = AlertSpeed;
}

void AKREnemyCharacter::SetChaseSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
}

void AKREnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (PawnExtensionComponent && EnemyASC)
	{
		if (PawnData)
		{
			PawnExtensionComponent->SetPawnData(PawnData);
		}

		PawnExtensionComponent->InitializeAbilitySystem(EnemyASC, this);
	}

	EnemyASC->AddLooseGameplayTag(KRTAG_ENEMY_IMMUNE_GRAPPLE);

	RegisterTagEvent();

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (LockOnSockets.Num() == 0)
		{
			const TArray<FName> AllSocketNames = MeshComp->GetAllSocketNames();
			for (const FName& SocketName : AllSocketNames)
			{
				if (SocketName.ToString().StartsWith(TEXT("LockOn_"), ESearchCase::IgnoreCase))
				{
					LockOnSockets.Add(SocketName);
				}
			}
		}
	}

	if (EnemyASC)
	{
		EnemyASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(
			this,
			&AKREnemyCharacter::OnGEAdded
		);
	}
}

void AKREnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (PawnExtensionComponent)
	{
		PawnExtensionComponent->HandleControllerChanged();
	}
}

UStateTreeComponent* AKREnemyCharacter::GetStateTreeComponent() const
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (!IsValid(AIC)) return nullptr;

	return AIC->FindComponentByClass<UStateTreeComponent>();
}

void AKREnemyCharacter::RegisterTagEvent()
{
	StateTags.Add(KRTAG_STATE_HASCC_STUN);
	StateTags.Add(KRTAG_ENEMY_ACTION_SLASH);
	StateTags.Add(KRTAG_ENEMY_AISTATE_COMBAT);
	StateTags.Add(KRTAG_ENEMY_AISTATE_ALERT);
	StateTags.Add(KRTAG_ENEMY_AISTATE_PATROL);
	StateTags.Add(KRTAG_ENEMY_AISTATE_CHASE);
	StateTags.Add(KRTAG_ENEMY_AISTATE_HITREACTION);
	StateTags.Add(KRTAG_ENEMY_AISTATE_DEAD);

	for (const FGameplayTag& Tag : StateTags)
	{
		EnemyASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AKREnemyCharacter::HandleTagEvent);
	}
}

void AKREnemyCharacter::HandleTagEvent(FGameplayTag Tag, int32 Count)
{
	if (!StateTags.Contains(Tag)) return;

	if (EnemyASC->HasMatchingGameplayTag(Tag) && Count > 0)
	{
		SetEnemyState(Tag);
	}
	else if (Count == 0)
	{
		ExternalGAEnded(Tag);
	}
}

void AKREnemyCharacter::SetEnemyState(FGameplayTag StateTag)
{
	UStateTreeComponent* EnemyST = GetStateTreeComponent();
	if (!EnemyST) return;

	FStateTreeEvent Event;
	Event.Tag = StateTag;

	EnemyST->SendStateTreeEvent(Event);
}

void AKREnemyCharacter::ExternalGAEnded(FGameplayTag Tag)
{
	if (!EnemyASC) return;

	for (FGameplayAbilitySpec& Spec : EnemyASC->GetActivatableAbilities())
	{
		if (Tag == KRTAG_STATE_HASCC_STUN)
		{
			if (Spec.Ability && Spec.Ability->IsA(UKRGA_Enemy_Stun::StaticClass()))
			{
				for (UGameplayAbility* Instance : Spec.GetAbilityInstances())
				{
					if (UKRGA_Enemy_Stun* StunGA = Cast<UKRGA_Enemy_Stun>(Instance))
					{
						StunGA->ExternalAbilityEnded();
						return;
					}
				}
			}
		}
		else if (Tag == KRTAG_ENEMY_AISTATE_ALERT)
		{
			if (Spec.Ability && Spec.Ability->IsA(UKRGA_Enemy_Alert::StaticClass()))
			{
				for (UGameplayAbility* Instance : Spec.GetAbilityInstances())
				{
					if (UKRGA_Enemy_Alert* AlertGA = Cast<UKRGA_Enemy_Alert>(Instance))
					{
						AlertGA->ExternalAbilityEnded();
						return;
					}
				}
			}
		}
	}
}

void AKREnemyCharacter::OnGEAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	UE_LOG(LogTemp, Error, TEXT("TakeDamage"));
	if (EnemyASC)
	{
		EnemyASC->AddLooseGameplayTag(KRTAG_ENEMY_AISTATE_HITREACTION);
	}
}
