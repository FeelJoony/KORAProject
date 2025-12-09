// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MassEntityTraitBase.h"
#include "MassEntityTemplateRegistry.h"
#include "Characters/Citizen/Fragments/KRCitizenAppearanceFragment.h"
#include "KRCitizenAppearanceTrait.generated.h"

UCLASS()
class KORAPROJECT_API UKRCitizenAppearanceTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

public:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};
