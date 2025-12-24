#include "Item/Weapons/KRMeleeWeapon.h"
#include "Equipment/KREquipmentInstance.h"
#include "Inventory/Fragment/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragment/InventoryFragment_SetStats.h"

AKRMeleeWeapon::AKRMeleeWeapon()
{
	// Shape Trace 기반 히트 감지로 전환됨에 따라 BoxCollision 제거
	// 히트 감지는 UKRGameplayAbility_MeleeAttack + UKRAnimNotifyState_MeleeHitCheck에서 처리
}

void AKRMeleeWeapon::ConfigureWeapon(UKRInventoryItemInstance* InInstance)
{
	Super::ConfigureWeapon(InInstance);

	// 근접 무기 특화 설정이 필요하면 여기서 (예: 무기별 이펙트 설정 등)
}