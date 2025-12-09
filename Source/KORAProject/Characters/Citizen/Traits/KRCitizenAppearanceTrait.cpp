// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Citizen/Traits/KRCitizenAppearanceTrait.h"
#include "Characters/Citizen/Fragments/KRCitizenAppearanceFragment.h"
#include "SubSystem/KRCitizenStreamingSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UKRCitizenAppearanceTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
    FKRCitizenAppearanceFragment& AppearanceFragment = BuildContext.AddFragment_GetRef<FKRCitizenAppearanceFragment>();

    FName ChosenRowName = NAME_None;
    if (UGameInstance* GI = World.GetGameInstance())
    {
        if (UKRCitizenStreamingSubsystem* StreamSubsystem = GI->GetSubsystem<UKRCitizenStreamingSubsystem>())
        {
            ChosenRowName = StreamSubsystem->GetRandomRowName();
        }
    }

    AppearanceFragment.AppearanceRowName = ChosenRowName;
}
