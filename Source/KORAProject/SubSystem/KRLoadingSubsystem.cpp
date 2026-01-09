#include "SubSystem/KRLoadingSubsystem.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SWeakWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Engine/GameViewportClient.h"

class SKRLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SKRLoadingScreen)
		: _AnimationSpeed(2.0f)
	{}
		SLATE_ARGUMENT(float, AnimationSpeed)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		AnimationSpeed = InArgs._AnimationSpeed;
		StartTime = FPlatformTime::Seconds();
		CurrentDisplayProgress = 0.0f;
		TargetProgress = 0.0f;

		ChildSlot
		[
			SNew(SBox)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.BorderBackgroundColor(FLinearColor::Black)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					// Throbber
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					[
						SNew(SCircularThrobber)
						.Radius(24.0f)
					]
					// 로딩 텍스트
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.Padding(0.0f, 16.0f, 0.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Loading...")))
						.ColorAndOpacity(FLinearColor::White)
						.Font(FCoreStyle::GetDefaultFontStyle("Bold", 14))
					]
					// 진행 바
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.Padding(0.0f, 24.0f, 0.0f, 0.0f)
					[
						SNew(SBox)
						.WidthOverride(400.0f)
						.HeightOverride(8.0f)
						[
							SAssignNew(ProgressBar, SProgressBar)
							.Percent(this, &SKRLoadingScreen::GetProgress)
							.FillColorAndOpacity(FLinearColor(0.2f, 0.6f, 1.0f, 1.0f))
							.BackgroundImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
							.FillImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
						]
					]
				]
			]
		];
		RegisterActiveTimer(0.0f, FWidgetActiveTimerDelegate::CreateSP(this, &SKRLoadingScreen::UpdateProgressAnimation));
	}

	EActiveTimerReturnType UpdateProgressAnimation(double InCurrentTime, float InDeltaTime)
	{
		float ElapsedTime = FPlatformTime::Seconds() - StartTime;
		float TimeBasedProgress = FMath::Clamp(ElapsedTime / 3.0f, 0.0f, 0.9f);

		if (!IsAsyncLoading())
		{
			TargetProgress = 1.0f;
		}
		else
		{
			TargetProgress = TimeBasedProgress;
		}
		
		CurrentDisplayProgress = FMath::FInterpTo(CurrentDisplayProgress, TargetProgress, InDeltaTime, AnimationSpeed);
		return EActiveTimerReturnType::Continue;
	}

	TOptional<float> GetProgress() const
	{
		return CurrentDisplayProgress;
	}

private:
	TSharedPtr<SProgressBar> ProgressBar;
	double StartTime = 0.0;
	float CurrentDisplayProgress = 0.0f;
	float TargetProgress = 0.0f;
	float AnimationSpeed = 2.0f;
};

void UKRLoadingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UKRLoadingSubsystem::Deinitialize()
{
	HideLoadingScreen();
	Super::Deinitialize();
}

UKRLoadingSubsystem& UKRLoadingSubsystem::Get()
{
	check(GEngine);
	UKRLoadingSubsystem* Subsystem = GEngine->GetEngineSubsystem<UKRLoadingSubsystem>();
	check(Subsystem);
	return *Subsystem;
}

void UKRLoadingSubsystem::ShowLoadingScreen()
{
	if (bIsVisible)
	{
		return;
	}

	if (!GEngine || !GEngine->GameViewport)
	{
		return;
	}

	LoadingScreenSlateWidget = SNew(SKRLoadingScreen);

	GEngine->GameViewport->AddViewportWidgetContent(
		SNew(SWeakWidget).PossiblyNullContent(LoadingScreenSlateWidget.ToSharedRef()),
		INT32_MAX
	);

	bIsVisible = true;
}

void UKRLoadingSubsystem::HideLoadingScreen()
{
	if (!bIsVisible || !LoadingScreenSlateWidget.IsValid())
	{
		return;
	}

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(LoadingScreenSlateWidget.ToSharedRef());
	}

	LoadingScreenSlateWidget.Reset();
	bIsVisible = false;
}

