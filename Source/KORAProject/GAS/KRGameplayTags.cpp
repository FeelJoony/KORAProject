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

/* State Tags */
UE_DEFINE_GAMEPLAY_TAG(KRTag_State_Acting_Attaking, "State.Acting.Attaking");

/* Player Tags */
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Weapon_Sword,	"Player.Weapon.Sword");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Weapon_Gun,		"Player.Weapon.Gun");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Can_Step,		"Player.Can.Step");
UE_DEFINE_GAMEPLAY_TAG(KRTag_Player_Can_Jump,		"Player.Can.Jump");

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