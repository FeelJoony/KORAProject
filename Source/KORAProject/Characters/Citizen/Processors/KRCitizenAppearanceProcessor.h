// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MassProcessor.h"
#include "MassEntityTypes.h"
#include "MassExecutionContext.h"
#include "MassActorSubsystem.h"

#include "KRCitizenAppearanceProcessor.generated.h"


UCLASS()
class KORAPROJECT_API UKRCitizenAppearanceProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
    UKRCitizenAppearanceProcessor();

protected:
    virtual void ConfigureQueries(const TSharedRef<FMassEntityManager>& EntityManager) override;
    virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

private:
    FMassEntityQuery AppearanceQuery;

    bool bAnyWorkDone = false;
};
