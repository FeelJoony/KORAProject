// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Citizen/Processors/KRCitizenAppearanceProcessor.h"

#include "MassCommonFragments.h"
#include "MassRepresentationFragments.h"
#include "MassEntityView.h"

#include "Characters/Citizen/Fragments/KRCitizenAppearanceFragment.h"
#include "Characters/Citizen/KRCitizenCharacter.h"
#include "SubSystem/KRCitizenStreamingSubsystem.h"
#include "Engine/World.h"

UKRCitizenAppearanceProcessor::UKRCitizenAppearanceProcessor()
{
	ProcessingPhase = EMassProcessingPhase::PostPhysics;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	bRequiresGameThreadExecution = true;
	bAutoRegisterWithProcessingPhases = true;
	QueryBasedPruning = EMassQueryBasedPruning::Never;

	AppearanceQuery.RegisterWithProcessor(*this);
}

void UKRCitizenAppearanceProcessor::ConfigureQueries(
	const TSharedRef<FMassEntityManager>& EntityManager)
{
	AppearanceQuery.Clear();

	AppearanceQuery.AddRequirement<FKRCitizenAppearanceFragment>(
		EMassFragmentAccess::ReadWrite,
		EMassFragmentPresence::All);

	AppearanceQuery.AddRequirement<FMassActorFragment>(
		EMassFragmentAccess::ReadWrite,
		EMassFragmentPresence::All);
}

void UKRCitizenAppearanceProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	UWorld* World = GetWorld();
	if (!World) return;
	UGameInstance* GI = World->GetGameInstance();
	if (!GI) return;

	UKRCitizenStreamingSubsystem* StreamSubsystem = GI->GetSubsystem<UKRCitizenStreamingSubsystem>();
	if (!StreamSubsystem || !StreamSubsystem->IsPreloadCompleted()) return;

	AppearanceQuery.ForEachEntityChunk(EntityManager, Context,
		[this, StreamSubsystem](FMassExecutionContext& Context)
		{
			TArrayView<FKRCitizenAppearanceFragment> AppearanceList = Context.GetMutableFragmentView<FKRCitizenAppearanceFragment>();
			const TArrayView<FMassActorFragment> ActorList = Context.GetMutableFragmentView<FMassActorFragment>();
			const int32 NumEntities = Context.GetNumEntities();

			for (int32 i = 0; i < NumEntities; ++i)
			{
				FKRCitizenAppearanceFragment& Appearance = AppearanceList[i];
				FMassActorFragment& ActorFragment = ActorList[i];

				if (Appearance.bInitialized) continue;

				AActor* Actor = ActorFragment.GetMutable();
				AKRCitizenCharacter* Citizen = Cast<AKRCitizenCharacter>(Actor);
				if (!Citizen) continue;

				const FKRCitizenAppearanceData* Row = StreamSubsystem->GetAppearanceRow(Appearance.AppearanceRowName);
				if (!Row)
				{
					UE_LOG(LogTemp, Error, TEXT("[CitizenProcessor] Row NOT FOUND: %s"), *Appearance.AppearanceRowName.ToString());
					continue;
				}

				Citizen->ApplyAppearanceFromRow(*Row);
				Appearance.bInitialized = true;

				bAnyWorkDone = true;
			}

		});

	if (bAnyWorkDone)
	{
		SetProcessingPhase(EMassProcessingPhase::MAX);
	}
}
