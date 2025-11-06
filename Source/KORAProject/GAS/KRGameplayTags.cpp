#include "GAS/KRGameplayTags.h"

namespace KRGameplayTags
{
	/* Input Tags */
	UE_DEFINE_GAMEPLAY_TAG(Input_Move, "Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Look, "Input.Look");

	UE_DEFINE_GAMEPLAY_TAG(Input_GASprint, "Input.GASprint");

	/* State Tags */
	UE_DEFINE_GAMEPLAY_TAG(State_Acting_Attaking, "State.Acting.Attaking");
	UE_DEFINE_GAMEPLAY_TAG(State_Acting_Sprint, "State.Acting.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(State_Acting_Steping, "State.Acting.Steping");
	UE_DEFINE_GAMEPLAY_TAG(State_Acting_Jumping, "State.Acting.Jumping");
	
	/*Action Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Player_Action_Step, "Player.Action.Step");
	UE_DEFINE_GAMEPLAY_TAG(Player_Action_Jump, "Player.Action.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Player_Can_Step, "Player.Can.Step");
	UE_DEFINE_GAMEPLAY_TAG(Player_Can_Jump, "Player.Can.Jump");

	/*Cooldown Tags*/
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Ability, "Cooldown.Ability");
}
