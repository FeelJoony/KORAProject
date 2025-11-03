// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/GameplayTags/KRProjectTagsModule.h"
#include "Item/GameplayTags/KRGameplayTags_Item.h"
#include "Item/GameplayTags/KRGameplayTags_Ability.h"
#include "Item/GameplayTags/KRGameplayTags_Shop.h"

#define LOCTEXT_NAMESPACE "FKORAProjectTagsModule"

void FKRProjectTagsModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("KORAProjectTags Module Loaded - Registering Gameplay Tags"));

	FItemGameplayTags::InitializeNativeTags();
	FAbilityGameplayTags::InitializeNativeTags();
	FShopGameplayTags::InitializeNativeTags();
}

void FKRProjectTagsModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("KORAProjectTags Module Unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FKRProjectTagsModule, KORAProjectTags)