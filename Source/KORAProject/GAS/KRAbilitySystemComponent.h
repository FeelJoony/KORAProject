#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "KRAbilitySystemComponent.generated.h"

class AActor;
class UKRAbilityTagRelationshipMapping;

UCLASS()
class KORAPROJECT_API UKRAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UKRAbilitySystemComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	void ClearAbilityInput();
	void SetTagRelationshipMapping(UKRAbilityTagRelationshipMapping* NewTagMapping);
	

protected:
	UPROPERTY()
	TObjectPtr<UKRAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;


	void TryActivateAbilitiesOnSpawn();
};
