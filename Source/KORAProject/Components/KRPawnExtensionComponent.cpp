#include "Components/KRPawnExtensionComponent.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "Data/DataAssets/KRPawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Equipment/KREquipmentManagerComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameplayTag/KRStateTag.h"
#include "GameplayTag/KRItemTypeTag.h"
#include "Net/UnrealNetwork.h"
#include "Player/KRPlayerState.h"
#include "SubSystem/KRInventorySubsystem.h"
#include "Inventory/KRInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRPawnExtensionComponent)

const FName UKRPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UKRPawnExtensionComponent::UKRPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
	bInventoryInitialized = false;
}

void UKRPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UKRPawnExtensionComponent, PawnData);
}

bool UKRPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	

	if (!CurrentState.IsValid() && DesiredState == KRTAG_STATE_INIT_SPAWNED)
	{
		if (Pawn) return true;
	}

	if (CurrentState == KRTAG_STATE_INIT_SPAWNED && DesiredState == KRTAG_STATE_INIT_DATAAVAILABLE)
	{
		if (!PawnData) return false;

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			if (!GetController<AController>()) return false;
		}

		return true;
	}
	else if (CurrentState == KRTAG_STATE_INIT_DATAAVAILABLE && DesiredState == KRTAG_STATE_INIT_DATAINITIALIZED)
	{
		if (!PawnData)
		{
			return false;
		}
		
		return Manager->HaveAllFeaturesReachedInitState(Pawn, KRTAG_STATE_INIT_DATAAVAILABLE);
	}
	else if (CurrentState == KRTAG_STATE_INIT_DATAINITIALIZED && DesiredState == KRTAG_STATE_INIT_GAMEPLAYREADY)
	{
		return true;
	}
	
	return false;
}

void UKRPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	// 상태 변경 시 추가 로직 필요하면 작성
}

void UKRPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == KRTAG_STATE_INIT_DATAAVAILABLE)
		{
			CheckDefaultInitialization();
		}
	}
}

void UKRPawnExtensionComponent::CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();

	if (!PawnData)
	{
		if (APawn* Pawn = GetPawn<APawn>())
		{
			if (AKRPlayerState* KRPS = Pawn->GetPlayerState<AKRPlayerState>())
			{
				if (const UKRPawnData* PlayerPawnData = KRPS->GetPawnData<UKRPawnData>())
				{
					SetPawnData(PlayerPawnData);
				}
			}
		}
	}

	if (PawnData && !bInventoryInitialized)
	{
		if (GetOwner()->HasAuthority())
		{
			InitializeInventory();
			bInventoryInitialized = true;
		}
	}
	
	static const TArray<FGameplayTag> StateChain = {
		KRTAG_STATE_INIT_SPAWNED,
		KRTAG_STATE_INIT_DATAAVAILABLE,
		KRTAG_STATE_INIT_DATAINITIALIZED,
		KRTAG_STATE_INIT_GAMEPLAYREADY
	};

	ContinueInitStateChain(StateChain);
}

void UKRPawnExtensionComponent::SetPawnData(const UKRPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

void UKRPawnExtensionComponent::InitializeAbilitySystem(UKRAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		if (UKRPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (PawnData)
	{
		InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);

		GrantedHandles.TakeFromAbilitySystem(AbilitySystemComponent);

		for (const UKRAbilitySet* AbilitySet : PawnData->AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
			}
		}
	}
	OnAbilitySystemInitialized.Broadcast();
}

void UKRPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	GrantedHandles.TakeFromAbilitySystem(AbilitySystemComponent);

	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent->SetAvatarActor(nullptr);
	}
	
	AbilitySystemComponent = nullptr;
	OnAbilitySystemUninitialized.Broadcast();
}

void UKRPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}

	CheckDefaultInitialization();
}

void UKRPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UKRPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UKRPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("KRPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	RegisterInitStateFeature();
}

void UKRPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	ensure(TryToChangeInitState(KRTAG_STATE_INIT_SPAWNED));
	CheckDefaultInitialization();
}

void UKRPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UKRPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

void UKRPawnExtensionComponent::InitializeInventory()
{
	if (!PawnData) return;
	if (GetOwner()->GetLocalRole() != ROLE_Authority) return;

	if (bInventoryInitialized) return;

	UWorld* World = GetWorld();
	if (!World) return;

	UGameInstance* GI = World->GetGameInstance();
	UKRInventorySubsystem* InvSubSystem = GI ? GI->GetSubsystem<UKRInventorySubsystem>() : nullptr;

	if (!InvSubSystem)
	{
		UE_LOG(LogTemp, Error, TEXT("[KRPawnExtension] InventorySubsystem Not Found!"));
		return;
	}

	// EquipmentManager 가져오기
	UKREquipmentManagerComponent* EquipComp = GetOwner()->FindComponentByClass<UKREquipmentManagerComponent>();

	for (const FGameplayTag& ItemTag : PawnData->DefaultInventoryItemTags)
	{
		if (ItemTag.IsValid())
		{
			UKRInventoryItemInstance* ItemInstance = InvSubSystem->AddItem(ItemTag, 1);

			// 장비 아이템이면 자동 장착
			if (EquipComp && ItemInstance)
			{
				const bool bIsEquipItem = ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_SWORD) ||
				                          ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_GUN) ||
				                          ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_HEAD) ||
				                          ItemTag.MatchesTag(KRTAG_ITEMTYPE_EQUIP_COSTUME);
				if (bIsEquipItem)
				{
					// Equippable Fragment가 없으면 추가
					if (!ItemInstance->FindFragmentByTag(KRTAG_FRAGMENT_ITEM_EQUIPPABLE))
					{
						InvSubSystem->AddFragment(ItemTag, KRTAG_FRAGMENT_ITEM_EQUIPPABLE);
					}

					EquipComp->EquipItem(ItemInstance);
				}
			}
		}
	}
	bInventoryInitialized = true;
}
