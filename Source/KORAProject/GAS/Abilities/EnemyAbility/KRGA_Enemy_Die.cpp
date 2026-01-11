#include "GAS/Abilities/EnemyAbility/KRGA_Enemy_Die.h"
#include "AI/KREnemyPawn.h"
#include "GAS/KRAbilitySystemComponent.h"
#include "GameplayTag/KREnemyTag.h"
#include "GameplayTag/KRShopTag.h"
#include "GameplayTag/KRItemTypeTag.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Data/EnemyDropDataStruct.h"
#include "Data/ItemDataStruct.h"
#include "SubSystem/KRDataTablesSubsystem.h"
#include "SubSystem/KRInventorySubsystem.h"
#include "Components/KRCurrencyComponent.h"
#include "Player/KRPlayerState.h"
#include "UI/Data/UIStruct/KRUIMessagePayloads.h"
#include "Kismet/GameplayStatics.h"

void UKRGA_Enemy_Die::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	if (DieMontage)
	{
		MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			DieMontage,
			1.f,
			NAME_None
		);

		OrganizeStateTag();
		ActivationDie();
		SpawnDrops();

		UGameplayMessageSubsystem& Subsystem = UGameplayMessageSubsystem::Get(GetWorld());

		FKillMonsterMessage Message;
		Message.MonsterIndex = 1;
		Subsystem.BroadcastMessage(KRTAG_ENEMY_AISTATE_DEAD, Message);
	}
}

void UKRGA_Enemy_Die::EndAbility(const FGameplayAbilitySpecHandle Handle, 
									const FGameplayAbilityActorInfo* ActorInfo, 
									const FGameplayAbilityActivationInfo ActivationInfo, 
									bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AKREnemyPawn* Enemy = Cast<AKREnemyPawn>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Cast<UKRAbilitySystemComponent>(Enemy->GetAbilitySystemComponent());
	if (!EnemyASC) return;

	if (EnemyASC->HasMatchingGameplayTag(KRTAG_ENEMY_AISTATE_DEAD))
	{
		EnemyASC->RemoveLooseGameplayTag(KRTAG_ENEMY_AISTATE_DEAD);
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);
	}
}

void UKRGA_Enemy_Die::OrganizeStateTag()
{
	AKREnemyPawn* Enemy = Cast<AKREnemyPawn>(GetAvatarActorFromActorInfo());
	UKRAbilitySystemComponent* EnemyASC = Cast<UKRAbilitySystemComponent>(Enemy->GetAbilitySystemComponent());
	if (!EnemyASC) return;

	const FGameplayTagContainer& CurrentTags = EnemyASC->GetOwnedGameplayTags();

	FGameplayTagContainer TagsToKeep;
	TagsToKeep.AddTag(KRTAG_ENEMY_IMMUNE_GRAPPLE);
	TagsToKeep.AddTag(KRTAG_ENEMY_AISTATE_DEAD);

	FGameplayTagContainer TagsToRemove = CurrentTags;
	TagsToRemove.RemoveTags(TagsToKeep);

	for (const FGameplayTag& Tag : TagsToRemove)
	{
		if (EnemyASC->HasMatchingGameplayTag(Tag))
		{
			EnemyASC->RemoveLooseGameplayTag(Tag);
		}
	}
}

void UKRGA_Enemy_Die::ActivationDie()
{
	MontageTask->OnCompleted.AddDynamic(this, &UKRGA_Enemy_Die::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UKRGA_Enemy_Die::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UKRGA_Enemy_Die::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &UKRGA_Enemy_Die::OnMontageEnded);
	MontageTask->ReadyForActivation();
}

void UKRGA_Enemy_Die::OnMontageEnded()
{
	AKREnemyPawn* Enemy = Cast<AKREnemyPawn>(GetAvatarActorFromActorInfo());
	USkeletalMeshComponent* Mesh = Enemy->GetMesh();
	if (Mesh)
	{
		Mesh->bPauseAnims = true;
		Mesh->bNoSkeletonUpdate = true;
	}

	AAIController* AIC = Cast<AAIController>(Enemy->GetController());
	if (IsValid(AIC))
	{
		AIC->BrainComponent->StopLogic(TEXT("Dead"));
		AIC->UnPossess();
	}

	if (UWorld* World = GetWorld())
	{
		TWeakObjectPtr<AActor> WeakEnemy = Enemy;

		World->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			[WeakEnemy]()
			{
				if (WeakEnemy.IsValid())
				{
					WeakEnemy->Destroy();
				}
			},
			DestroyDelay,
			false
		);
	}
}

void UKRGA_Enemy_Die::ExternalAbilityEnded()
{
}

void UKRGA_Enemy_Die::SpawnDrops()
{
	AKREnemyPawn* Enemy = Cast<AKREnemyPawn>(GetAvatarActorFromActorInfo());
	if (!Enemy) return; 

	UWorld* World = GetWorld();
	if (!World) return;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!PlayerPawn) return;

	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return;

	UKRDataTablesSubsystem* DataSubsystem = GI->GetSubsystem<UKRDataTablesSubsystem>();
	if (!DataSubsystem) return;

	FGameplayTag EnemyTag = Enemy->GetEnemyTag();
	FEnemyDropDataStruct* DropData = DataSubsystem->GetData<FEnemyDropDataStruct>(EnemyTag);
	if (!DropData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EnemyDie] No drop data found for enemy: %s"), *EnemyTag.ToString());
		return;
	}
	
	UKRCurrencyComponent* CurrencyComp = nullptr;
	if (APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController()))
	{
		if (AKRPlayerState* PS = PC->GetPlayerState<AKRPlayerState>())
		{
			CurrencyComp = PS->GetCurrencyComponentSet();
		}
	}
	
	UKRInventorySubsystem* InventorySubsystem = GI->GetSubsystem<UKRInventorySubsystem>();
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);

	if (DropData->GearingMax > 0 && CurrencyComp)
	{
		int32 GearingMin10 = DropData->GearingMin / 10;
		int32 GearingMax10 = DropData->GearingMax / 10;
		int32 GearingAmount = FMath::RandRange(GearingMin10, GearingMax10) * 10;

		if (GearingAmount > 0)
		{
			CurrencyComp->AddCurrency(KRTAG_CURRENCY_PURCHASE_GEARING, GearingAmount);
			FKRUIMessage_ItemLog GearingLogMsg;
			GearingLogMsg.EventType = EItemLogEvent::AcquiredNormal;
			GearingLogMsg.ItemTag = KRTAG_CURRENCY_PURCHASE_GEARING;
			GearingLogMsg.AcquiredQuantity = GearingAmount;
			MessageSubsystem.BroadcastMessage(FKRUIMessageTags::ItemLog(), GearingLogMsg);
		}
	}

	float CorbyteChance = FMath::RandRange(DropData->CorbyteChanceMin, DropData->CorbyteChanceMax);
	float CorbyteRoll = FMath::FRand() * 100.f;
	if (CorbyteRoll <= CorbyteChance && DropData->CorbyteAmount > 0 && CurrencyComp)
	{
		CurrencyComp->AddCurrency(KRTAG_CURRENCY_SKILL_CORBYTE, DropData->CorbyteAmount);
		FKRUIMessage_ItemLog CorbyteLogMsg;
		CorbyteLogMsg.EventType = EItemLogEvent::AcquiredNormal;
		CorbyteLogMsg.ItemTag = KRTAG_CURRENCY_SKILL_CORBYTE;
		CorbyteLogMsg.AcquiredQuantity = DropData->CorbyteAmount;
		MessageSubsystem.BroadcastMessage(FKRUIMessageTags::ItemLog(), CorbyteLogMsg);
	}

	if (DropData->DroppableItems.Num() > 0 && DropData->GuaranteedItemCount > 0 && InventorySubsystem)
	{
		float TotalChance = DropData->CommonChance + DropData->UncommonChance + DropData->RareChance;
		if (TotalChance <= 0.f)
		{
			TotalChance = 100.f;
		}

		for (int32 i = 0; i < DropData->GuaranteedItemCount; ++i)
		{
			float RarityRoll = FMath::FRand() * TotalChance;
			FGameplayTag TargetRarity;

			if (RarityRoll < DropData->CommonChance)
			{
				TargetRarity = KRTAG_RARITY_COMMON;
			}
			else if (RarityRoll < DropData->CommonChance + DropData->UncommonChance)
			{
				TargetRarity = KRTAG_RARITY_UNCOMMON;
			}
			else
			{
				TargetRarity = KRTAG_RARITY_RARE;
			}

			int32 ItemIndex = FMath::RandRange(0, DropData->DroppableItems.Num() - 1);
			const FDropItemEntry& SelectedItem = DropData->DroppableItems[ItemIndex];
			int32 ItemCount = FMath::RandRange(SelectedItem.MinCount, SelectedItem.MaxCount);
			if (ItemCount > 0 && SelectedItem.ItemTag.IsValid())
			{
				InventorySubsystem->AddItem(SelectedItem.ItemTag, ItemCount);
				FItemDataStruct* ItemData = DataSubsystem->GetData<FItemDataStruct>(SelectedItem.ItemTag);

				FKRUIMessage_ItemLog ItemLogMsg;
				ItemLogMsg.EventType = EItemLogEvent::AcquiredNormal;
				ItemLogMsg.ItemTag = SelectedItem.ItemTag;
				ItemLogMsg.AcquiredQuantity = ItemCount;
				ItemLogMsg.NewTotalQuantity = InventorySubsystem->GetItemCountByTag(SelectedItem.ItemTag);
				if (ItemData)
				{
					ItemLogMsg.ItemNameKey = ItemData->DisplayNameKey;
				}
				MessageSubsystem.BroadcastMessage(FKRUIMessageTags::ItemLog(), ItemLogMsg);
			}
		}
	}
}