#include "Slate/MonsterBlueprintViewport.h"

#include "SlateOptMacros.h"
#include "Components/DirectionalLightComponent.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FMonsterBlueprintViewportClient::FMonsterBlueprintViewportClient(FAdvancedPreviewScene& InPreviewScene, const TSharedRef<SMonsterBlueprintViewport>& InViewport)
	: FEditorViewportClient(nullptr, &InPreviewScene, StaticCastSharedRef<SEditorViewport>(InViewport)), AdvancedPreviewScene(&InPreviewScene)
{
	SetViewMode(VMI_Lit);

	SetRealtime(true);

	EngineShowFlags.Game = 0;
	EngineShowFlags.SetSnap(0);

	DrawHelper.bDrawGrid = true;
	DrawHelper.bDrawWorldBox = false;
	DrawHelper.bDrawKillZ = false;
	DrawHelper.GridColorAxis = FColor(70, 70, 70);
	DrawHelper.GridColorMajor = FColor(40, 40, 40);
	DrawHelper.GridColorMinor = FColor(20, 20, 20);

	SetViewLocation(FVector(-400, 0, 200));
	SetViewRotation(FRotator(-15, 0, 0));

	ViewFOV = 90.f;
}

FMonsterBlueprintViewportClient::~FMonsterBlueprintViewportClient()
{
	if (PreviewActor.IsValid())
	{
		PreviewActor->Destroy();
	}
}

void FMonsterBlueprintViewportClient::SetPreviewBlueprint(UBlueprint* InBlueprint)
{
	if (!InBlueprint || !InBlueprint->GeneratedClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Blueprint or GeneratedClass"));
		return;
	}

	UWorld* World = AdvancedPreviewScene->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid World"));
		return;
	}

	if (PreviewActor.IsValid())
	{
		PreviewActor->Destroy();
		PreviewActor = nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	SpawnParams.ObjectFlags = RF_Transient;

	AActor* NewActor = World->SpawnActor<AActor>(
		InBlueprint->GeneratedClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		SpawnParams);

	if (NewActor)
	{
		PreviewActor = NewActor;

		NewActor->RerunConstructionScripts();

		UE_LOG(LogTemp, Log, TEXT("Preview Actor Created: %s"), *NewActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn preview actor"));
	}
}

void FMonsterBlueprintViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FEditorViewportClient::Draw(View, PDI);

	if (PreviewActor.IsValid())
	{
		FVector Origin = PreviewActor->GetActorLocation();

		DrawCoordinateSystem(PDI, Origin, FRotator::ZeroRotator, 50.f, SDPG_World, 1.f);
	}
}

void FMonsterBlueprintViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	if (AdvancedPreviewScene)
	{
		AdvancedPreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

void SMonsterBlueprintViewport::Construct(const FArguments& InArgs)
{
	PreviewScene = MakeShared<FAdvancedPreviewScene>(
		FPreviewScene::ConstructionValues()
			.SetEditor(true)
			.ShouldSimulatePhysics(false)
			.SetCreatePhysicsScene(false));

	PreviewScene->SetFloorVisibility(false, false);
	PreviewScene->SetEnvironmentVisibility(true, true);

	UDirectionalLightComponent* DirectionalLight = PreviewScene->DirectionalLight;
	DirectionalLight->SetMobility(EComponentMobility::Movable);
	DirectionalLight->CastShadows = false;
	DirectionalLight->CastStaticShadows = false;
	DirectionalLight->CastDynamicShadows = false;
	DirectionalLight->Intensity = 3.f;

	SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SMonsterBlueprintViewport::SetBlueprint(UBlueprint* InBlueprint)
{
	CurrentBlueprint = InBlueprint;

	if (ViewportClient.IsValid())
	{
		ViewportClient->SetPreviewBlueprint(InBlueprint);
	}
}

TSharedRef<class SEditorViewport> SMonsterBlueprintViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SMonsterBlueprintViewport::GetExtenders() const
{
	return MakeShared<FExtender>();
}

void SMonsterBlueprintViewport::OnFloatingButtonClicked()
{
	
}

TSharedRef<FEditorViewportClient> SMonsterBlueprintViewport::MakeEditorViewportClient()
{
	ViewportClient = MakeShared<FMonsterBlueprintViewportClient>(
		*PreviewScene.Get(),
		SharedThis(this));

	return ViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SMonsterBlueprintViewport::MakeViewportToolbar()
{
	return nullptr; // SNew(SCommonEditorViewportToolbarBase, SharedThis(this)).AddRealtimeButton(false);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
