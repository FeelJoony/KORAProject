#include "Quest/KRQuestAcceptTriggerBox.h"

#include "Characters/KRHeroCharacter.h"
#include "Subsystem/KRQuestSubsystem.h"
#include "Components/ShapeComponent.h"

AKRQuestAcceptTriggerBox::AKRQuestAcceptTriggerBox()
{
	// TriggerBox는 보이지 않으므로 Ray Tracing 불필요
	// Destroy() 호출 시 RT geometry 크래시 방지
	if (UShapeComponent* CollisionComp = GetCollisionComponent())
	{
		CollisionComp->SetVisibleInRayTracing(false);
	}
}

void AKRQuestAcceptTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (AKRHeroCharacter* Character = Cast<AKRHeroCharacter>(OtherActor))
	{
		UKRQuestSubsystem& QuestSubsystem = UKRQuestSubsystem::Get(GetWorld());
		QuestSubsystem.AcceptQuest(QuestIndex);

		// Destroy() 대신 비활성화 + 지연 삭제
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetLifeSpan(0.5f);  // 0.5초 후 안전하게 삭제
	}
}
