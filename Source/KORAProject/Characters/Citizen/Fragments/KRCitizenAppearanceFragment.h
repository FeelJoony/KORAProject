// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MassEntityTypes.h"
#include "KRCitizenAppearanceFragment.generated.h"

USTRUCT()
struct KORAPROJECT_API FKRCitizenAppearanceFragment : public FMassFragment
{
	GENERATED_BODY()

	UPROPERTY() FName AppearanceRowName;
	UPROPERTY() bool bInitialized = false;
};
