#include "SubSystem/KRUserSubSystem.h"
#include "Contents/KRInventory.h"

void UKRUserSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	AddContents<UKRInventory>();
}

void UKRUserSubSystem::Deinitialize()
{
	RemoveContents<UKRInventory>();
}
