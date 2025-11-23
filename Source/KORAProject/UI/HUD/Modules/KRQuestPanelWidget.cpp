// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Modules/KRQuestPanelWidget.h"
#include "CommonTextBlock.h"

void UKRQuestPanelWidget::UpdateQuestPanel(FText InObjective)
{
    if (Objective)
    {
        Objective->SetText(InObjective);
    }
}
