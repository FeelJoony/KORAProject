#include "GAS/KRAbilityTagRelationshipMapping.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRAbilityTagRelationshipMapping)

void UKRAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
	for (const FKRAbilityTagRelationship& Relationship : AbilityTagRelationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			if (OutTagsToBlock)
			{
				OutTagsToBlock->AppendTags(Relationship.ActivationBlockedTags);
			}
			if (OutTagsToCancel)
			{
				OutTagsToCancel->AppendTags(Relationship.AbilityTagsToCancel);
			}
		}
	}
}

void UKRAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
	FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
	for (const FKRAbilityTagRelationship& Relationship : AbilityTagRelationships)
	{
		if (AbilityTags.HasTag(Relationship.AbilityTag))
		{
			if (OutActivationRequired)
			{
				OutActivationRequired->AppendTags(Relationship.ActivationRequiredTags);
			}
			if (OutActivationBlocked)
			{
				OutActivationBlocked->AppendTags(Relationship.ActivationBlockedTags);
			}
		}
	}
}

bool UKRAbilityTagRelationshipMapping::IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags,
	const FGameplayTag& ActionTag) const
{
	for (const FKRAbilityTagRelationship& Relationship : AbilityTagRelationships)
	{
		if (Relationship.AbilityTag == ActionTag && Relationship.AbilityTagsToCancel.HasAny(AbilityTags))
		{
			return true;
		}
	}
	return false;
}
