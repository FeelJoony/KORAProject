#include "KREnemyPawn.h"

#include "KRAIEnemyController.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/KRCombatComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Components/WidgetComponent.h"
#include "Data/EnemyAttributeDataStruct.h"
#include "Data/EnemyDataStruct.h"
#include "GameplayTag/KREnemyTag.h"
#include "GameplayTag/KRStateTag.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GAS/Abilities/KRGameplayAbility.h"
#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Hit.h"
#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Stun.h"
#include "GAS/AttributeSets/KRCombatCommonSet.h"
#include "GAS/AttributeSets/KREnemyAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "Subsystem/KRDataTablesSubsystem.h"
#include "UI/Components/KREnemyHPWidget.h"


AKREnemyPawn::AKREnemyPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AKRAIEnemyController::StaticClass();

	PrimaryActorTick.bCanEverTick = false;

	//CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCylinder"));
	//Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh0"));
	//ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	EnemyASC = CreateDefaultSubobject<UKRAbilitySystemComponent>(TEXT("EnemyASC"));
	CombatComponent = CreateDefaultSubobject<UKRCombatComponent>(TEXT("CombatComponent"));
	
	// SetRootComponent(CapsuleComponent);
	// Mesh->SetupAttachment(CapsuleComponent);
	// ArrowComponent->SetupAttachment(CapsuleComponent);
	
	CombatCommonSet = CreateDefaultSubobject<UKRCombatCommonSet>(TEXT("CombatCommonSet"));
	EnemyAttributeSet = CreateDefaultSubobject<UKREnemyAttributeSet>(TEXT("EnemyAttributeSet"));
}

UAbilitySystemComponent* AKREnemyPawn::GetAbilitySystemComponent() const
{
	return EnemyASC;
}

void AKREnemyPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializeComponents();
}

void AKREnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	if (EnemyASC)
	{
		HPWidgetComp = FindComponentByClass<UWidgetComponent>();
		if (!HPWidgetComp) return;

		UKREnemyHPWidget* HPWidget = Cast<UKREnemyHPWidget>(HPWidgetComp->GetUserWidgetObject());
		if (!HPWidget) return;

		HPWidget->InitFromASC(EnemyASC, this);

		EnemyASC->AddLooseGameplayTags(DefaultTags);
	}
}

void AKREnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HPWidgetComp) return;

	APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!Cam) return;

	FVector CamLocation = Cam->GetCameraLocation();
	FRotator LookAtRot = (CamLocation - HPWidgetComp->GetComponentLocation()).Rotation();

	LookAtRot.Pitch = 0.0f;
	LookAtRot.Roll = 0.0f;

	HPWidgetComp->SetWorldRotation(LookAtRot);
}

void AKREnemyPawn::InitializeComponents()
{
	if (EnemyASC == nullptr)
	{
		return;
	}

	EnemyASC->InitAbilityActorInfo(this, this);
	
	if (!EnemyTag.IsValid())
	{
		return;
	}

	const FEnemyDataStruct* EnemyDataStruct = nullptr;
	const FEnemyAttributeDataStruct* EnemyAttributeDataStruct = nullptr;

	UWorld* World = GetWorld();
	if (World == nullptr || World->GetGameInstance() == nullptr)
	{
		static const FString TablePath = TEXT("/Game/Data/DataTables");

		static const FString EnemyTableName = TEXT("EnemyData");
		static const FString EnemyAbilityTableName = TEXT("EnemyAbilityData");
		static const FString EnemyAttributeTableName = TEXT("EnemyAttributeData");

		EnemyDataStruct = FindDataByLoad<FEnemyDataStruct>(FPaths::Combine(TablePath, EnemyTableName));
		EnemyAttributeDataStruct = FindDataByLoad<FEnemyAttributeDataStruct>(FPaths::Combine(TablePath, EnemyAttributeTableName));
	}
	else
	{
		UKRDataTablesSubsystem& DataTablesSubsystem = UKRDataTablesSubsystem::Get(this);
		
		EnemyDataStruct = DataTablesSubsystem.GetData<FEnemyDataStruct>(EnemyTag);
		EnemyAttributeDataStruct = DataTablesSubsystem.GetData<FEnemyAttributeDataStruct>(EnemyTag);
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : ApplyAbilityClasses)
	{
		EnemyASC->K2_GiveAbility(AbilityClass);
	}

	if (EnemyAttributeDataStruct)
	{
		CombatCommonSet->SetMaxHealth(EnemyAttributeDataStruct->MaxHealth);
		CombatCommonSet->SetCurrentHealth(EnemyAttributeDataStruct->MaxHealth);
		CombatCommonSet->SetAttackPower(EnemyAttributeDataStruct->AttackPower);
		CombatCommonSet->SetDefensePower(EnemyAttributeDataStruct->DefensePower);
		CombatCommonSet->SetDealDamageMult(EnemyAttributeDataStruct->DealDamageMultiply);
		CombatCommonSet->SetTakeDamageMult(EnemyAttributeDataStruct->TakeDamageMultiply);
		CombatCommonSet->SetDamageTaken(EnemyAttributeDataStruct->DamageTaken);

		EnemyAttributeSet->SetCanAttackRange(EnemyAttributeDataStruct->CanAttackRange);
		EnemyAttributeSet->SetEnterRageStatusRate(EnemyAttributeDataStruct->EnterRageStatusRate);
	}

	// CurrentHealth 변경 감지 바인딩
	BindHealthChangedDelegate();

	// 태그 이벤트 등록
	RegisterTagEvent();
}

bool AKREnemyPawn::TryActivateAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (!EnemyASC)
	{
		return false;
	}

	FGameplayAbilitySpec* Spec = EnemyASC->FindAbilitySpecFromClass(AbilityClass);
	if (Spec == nullptr)
	{
		return false;
	}

	return EnemyASC->TryActivateAbility(Spec->Handle);
}

void AKREnemyPawn::BindHealthChangedDelegate()
{
	if (!EnemyASC)
	{
		return;
	}

	// 이미 바인딩되어 있으면 제거
	if (HealthChangedDelegateHandle.IsValid())
	{
		EnemyASC->GetGameplayAttributeValueChangeDelegate(
			UKRCombatCommonSet::GetCurrentHealthAttribute()
		).Remove(HealthChangedDelegateHandle);
		HealthChangedDelegateHandle.Reset();
	}

	// CurrentHealth 변경 델리게이트에 콜백 바인딩
	HealthChangedDelegateHandle = EnemyASC->GetGameplayAttributeValueChangeDelegate(
		UKRCombatCommonSet::GetCurrentHealthAttribute()
	).AddUObject(this, &AKREnemyPawn::OnHealthChanged);
}

void AKREnemyPawn::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	const float OldHealth = Data.OldValue;
	const float NewHealth = Data.NewValue;

	// 체력이 감소했을 때만 Hit Ability 실행
	if (NewHealth < OldHealth && EnemyASC)
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.HitReaction")));
		EnemyASC->TryActivateAbilitiesByTag(TagContainer);

		if (AController* AIController = GetController())
		{
			if (UStateTreeAIComponent* StateTreeComp = Cast<UStateTreeAIComponent>(AIController->GetComponentByClass<UStateTreeAIComponent>()))
			{
				if (NewHealth <= 0.f)
				{
					StateTreeComp->SendStateTreeEvent(KRTAG_ENEMY_AISTATE_DEAD);
				}
				else
				{
					StateTreeComp->SendStateTreeEvent(KRTAG_ENEMY_AISTATE_HITREACTION);
				}
			}
		}

		if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			UAISense_Damage::ReportDamageEvent(
				GetWorld(),
				this,
				PlayerCharacter,
				OldHealth - NewHealth,
				PlayerCharacter->GetActorLocation(),
				GetActorLocation()
			);	
		}
	}
}

void AKREnemyPawn::RegisterTagEvent()
{
	if (!EnemyASC) return;

	StateTags.AddTag(KRTAG_STATE_HASCC_STUN);
	StateTags.AddTag(KRTAG_ENEMY_ACTION_SLASH);
	StateTags.AddTag(KRTAG_ENEMY_AISTATE_COMBAT);
	StateTags.AddTag(KRTAG_ENEMY_AISTATE_ALERT);
	StateTags.AddTag(KRTAG_ENEMY_AISTATE_PATROL);
	StateTags.AddTag(KRTAG_ENEMY_AISTATE_CHASE);
	StateTags.AddTag(KRTAG_ENEMY_AISTATE_HITREACTION);
	StateTags.AddTag(KRTAG_ENEMY_AISTATE_DEAD);

	for (const FGameplayTag& Tag : StateTags)
	{
		EnemyASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &AKREnemyPawn::HandleTagEvent);
	}
}

void AKREnemyPawn::HandleTagEvent(FGameplayTag Tag, int32 Count)
{
	if (!StateTags.HasTag(Tag)) return;

	if (EnemyASC->HasMatchingGameplayTag(Tag) && Count > 0)
	{
		SetEnemyState(Tag);
	}
	else if (Count == 0)
	{
		ExternalGAEnded(Tag);
	}
}

void AKREnemyPawn::SetEnemyState(FGameplayTag StateTag)
{
	AController* AIController = GetController();
	if (!AIController) return;

	UStateTreeAIComponent* StateTreeComp = AIController->GetComponentByClass<UStateTreeAIComponent>();
	if (!StateTreeComp) return;

	FStateTreeEvent Event;
	Event.Tag = StateTag;

	StateTreeComp->SendStateTreeEvent(Event);
}

void AKREnemyPawn::ExternalGAEnded(FGameplayTag Tag)
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
	}
}
