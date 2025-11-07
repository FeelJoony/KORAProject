#include "GAS/KRGameplayTags.h"

namespace KRGameplayTags
{
	/* Input Tags */
	UE_DEFINE_GAMEPLAY_TAG(Input_Move,			"Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Look,			"Input.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Equip_Sword,	"Input.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Input_Equip_Gun,		"Input.Equip.Gun");
	UE_DEFINE_GAMEPLAY_TAG(Input_Unequip_Sword, "Input.Unequip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Input_Unequip_Gun,	"Input.Unequip.Gun");
	
	
	/* State Tags */
	UE_DEFINE_GAMEPLAY_TAG(State_Acting_Attaking, "State.Acting.Attaking");


	/* Player Tags */
	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Sword, "Player.Weapon.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Player_Weapon_Gun,	"Player.Weapon.Gun");
	UE_DEFINE_GAMEPLAY_TAG(Player_Can_Step,		"Player.Can.Step");
	UE_DEFINE_GAMEPLAY_TAG(Player_Can_Jump,		"Player.Can.Jump");

	/* Event Tags */
	UE_DEFINE_GAMEPLAY_TAG(Event_Equip_Sword,	"Event.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Event_Unequip_Sword, "Event.Unequip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Event_Equip_Gun,		"Event.Equip.Gun");
	UE_DEFINE_GAMEPLAY_TAG(Event_Unequip_Gun,	"Event.Unequip.Gun");

	/* Ability Tags */
	UE_DEFINE_GAMEPLAY_TAG(Ability_Equip_Sword,	  "Ability.Equip.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Unequip_Sword, "Ability.Unequip.Sword");	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Equip_Gun,	  "Ability.Equip.Gun");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Unequip_Gun,	  "Ability.Unequip.Gun");

	
}
