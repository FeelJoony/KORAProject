#include "GAS/KRGameplayTags.h"



/* Input Tags */
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_Move,				 "Input.Move");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_Look,				 "Input.Look");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_Equip_Sword,			 "Input.Equip.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_Equip_Gun,			 "Input.Equip.Gun");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_Unequip_Sword,		 "Input.Unequip.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_Unequip_Gun,			 "Input.Unequip.Gun");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_LightAttack_Sword,	 "Input.LightAttack.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_ChargeAttack_Sword,	 "Input.ChargeAttack.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_GASprint,			 "Input.GASprint");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Input_GAInteract,			 "Input.GAInteract");

/* State Tags */
UE_DEFINE_GAMEPLAY_TAG(KRTag_State_Acting_Attaking,		"State.Acting.Attaking");
UE_DEFINE_GAMEPLAY_TAG(KRTag_State_Acting_Sprint,		"State.Acting.Sprinting");
UE_DEFINE_GAMEPLAY_TAG(KRTag_State_Acting_Steping,		"State.Acting.Steping");
UE_DEFINE_GAMEPLAY_TAG(KRTag_State_Acting_Jumping,		"State.Acting.Jumping");
UE_DEFINE_GAMEPLAY_TAG(KRTag_State_Acting_Laddering,	"State.Acting.Laddering");
UE_DEFINE_GAMEPLAY_TAG(KRTag_State_Acting_Interacting,	"State.Acting.Interacing");

/* Player Tags */
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Weapon_Sword,	"Player.Weapon.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Weapon_Gun,		"Player.Weapon.Gun");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Can_Step,		"Player.Can.Step");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Can_Jump,		"Player.Can.Jump");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Action_Step,	"Player.Action.Step");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Action_Jump,	"Player.Action.Jump");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Action_Ladder,	"Player.Action.Ladder");

/* Event Tags */
UE_DEFINE_GAMEPLAY_TAG(KRTag_Event_Equip_Sword,		"Event.Equip.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Event_Unequip_Sword,	"Event.Unequip.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Event_Equip_Gun,		"Event.Equip.Gun");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Event_Unequip_Gun,		"Event.Unequip.Gun");

/* Ability Tags */
UE_DEFINE_GAMEPLAY_TAG(KRTag_Ability_Equip_Sword,			"Ability.Equip.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Ability_Unequip_Sword,			"Ability.Unequip.Sword");	
UE_DEFINE_GAMEPLAY_TAG(KRTag_Ability_Equip_Gun,				"Ability.Equip.Gun");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Ability_Unequip_Gun,			"Ability.Unequip.Gun");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Ability_Attack_Light_Sword,	"Ability.Attack.Light.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Ability_Attack_Charge_Sword,	"Ability.Attack.Charge.Sword");

/*Cooldown Tags*/
UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability, "Cooldown.Ability");
UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability_Jump, "Cooldown.Ability.Jump");
