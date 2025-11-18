#include "GAS/KRGameplayTags.h"

namespace KRGameplayTags
{
	/* Input Tags */
	UE_DEFINE_GAMEPLAY_TAG(Input_Move, "Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Look, "Input.Look");

	// ----- Ability -----

	/* State Tags */
	UE_DEFINE_GAMEPLAY_TAG(State_Acting_Attaking, "State.Acting.Attaking");
	
	UE_DEFINE_GAMEPLAY_TAG(Player_Can_Step, "Player.Can.Step");
	UE_DEFINE_GAMEPLAY_TAG(Player_Can_Jump, "Player.Can.Jump");
	
	// ----- PoolTags -----
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Pool_AlwaysOn, "Pool.AlwaysOn", "상점 상시 품목");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Pool_Rotation_R1, "Pool.Rotation.R1", "상점 1 품목");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Pool_Rotation_R2, "Pool.Rotation.R2", "상점 2 품목");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Pool_Rotation_R3, "Pool.Rotation.R3", "상점 3 품목");

	// ===== Rarity =====
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Rarity_Common, "Rarity.Common", "Rarity Common");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Rarity_Uncommon, "Rarity.Uncommon", "Rarity Uncommon");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Rarity_Rare, "Rarity.Rare", "Rarity Rare");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Rarity_Epic, "Rarity.Epic", "Rarity Epic");
}