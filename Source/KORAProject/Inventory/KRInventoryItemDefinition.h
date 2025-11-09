#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
//#include "KRInventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UKRInventoryItemInstance;
struct FFrame;

//UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
//class KORAPROJECT_API UKRInventoryItemDefinition : public UObject
//{
//	GENERATED_BODY()
//
//public:
//	virtual void OnInstanceCreated(UKRInventoryItemInstance* Instance) const {}
//};
//
//UCLASS(Blueprintable, Const, Abstract)
//class UKRInventoryItemDefinition : public UObject
//{
//	GENERATED_BODY()
//
//public:
//	UKRInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
//	FText DisplayName;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
//	TArray<TObjectPtr<UKRInventoryItemFragment>> Fragments;
//
//public:
//	const UKRInventoryItemFragment* FindFragmentByClass(TSubclassOf<UKRInventoryItemFragment> FragmentClass) const;
//};
//
//UCLASS()
//class UKRInventoryFunctionLibrary : public UBlueprintFunctionLibrary
//{
//	GENERATED_BODY()
//
//	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
//	static const UKRInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UKRInventoryItemDefinition> ItemDef, TSubclassOf<UKRInventoryItemFragment> FragmentClass);
//};
