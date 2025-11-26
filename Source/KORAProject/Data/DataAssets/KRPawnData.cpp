#include "Data/DataAssets/KRPawnData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(KRPawnData)

UKRPawnData::UKRPawnData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PawnClass = nullptr;
	InputConfig = nullptr;
	TagRelationshipMapping = nullptr;
}
