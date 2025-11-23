// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "KRProgressBarMessages.generated.h"

USTRUCT(BlueprintType)
struct FKRProgressBarMessages
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly) TWeakObjectPtr<AActor> TargetActor;

    UPROPERTY(BlueprintReadOnly) FGameplayTag ProgressBarTag; // HP, Stamina, CoreDrive

    UPROPERTY(BlueprintReadOnly) float NewValue = 0.f;
    UPROPERTY(BlueprintReadOnly) float MaxValue = 0.f;
    UPROPERTY(BlueprintReadOnly) float Delta = 0.f; // - Damage, + Recovery
};