#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "AdvancedPreviewScene.h"

class FMonsterBlueprintViewportClient : public FEditorViewportClient
{
public:
	FMonsterBlueprintViewportClient(FAdvancedPreviewScene& InPreviewScene, const TSharedRef<class SMonsterBlueprintViewport>& InViewport);

	virtual ~FMonsterBlueprintViewportClient() override;

	void SetPreviewBlueprint(UBlueprint* InBlueprint);

	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;

	virtual void Tick(float DeltaSeconds) override;

private:
	TWeakObjectPtr<AActor> PreviewActor;
	FAdvancedPreviewScene* AdvancedPreviewScene;
};

class KORACUSTOMEDITORS_API SMonsterBlueprintViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SMonsterBlueprintViewport) {}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetBlueprint(UBlueprint* InBlueprint);

	// ICommonEditorViewportToolbarInfoProvider interface
	virtual TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;

private:
	TSharedPtr<FAdvancedPreviewScene> PreviewScene;
	TSharedPtr<FMonsterBlueprintViewportClient> ViewportClient;
	TWeakObjectPtr<UBlueprint> CurrentBlueprint;
	
};
