#include "Animation/KRHeroLinkedAnimLayer.h"
#include "Animation/KRHeroAnimInstance.h"

UKRHeroAnimInstance* UKRHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
	return Cast<UKRHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
