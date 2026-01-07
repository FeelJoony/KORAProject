
#include "KRCombatTag.h"

// ============================================================
// COMBAT TAGS - For CombatActionData
// ============================================================
UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_ATTACK_LIGHT, "Combat.Attack.Light");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_ATTACK_HEAVY, "Combat.Attack.Heavy");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_ATTACK_CHARGE, "Combat.Attack.Charge");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_ATTACK_AERIAL, "Combat.Attack.Aerial");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_ATTACK_FINISHER, "Combat.Attack.Finisher");

UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_DEFENSE_GUARD, "Combat.Defense.Guard");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_DEFENSE_PARRY, "Combat.Defense.Parry");

UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_GUN_FIRE, "Combat.Gun.Fire");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_GUN_RELOAD, "Combat.Gun.Reload");

UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_DODGE, "Combat.Dodge");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_COMBAT_ROLL, "Combat.Roll");

// ============================================================
// HIT REACTION TAGS
// ============================================================
UE_DEFINE_GAMEPLAY_TAG(KRTAG_HITREACTION_NORMAL, "HitReaction.Normal");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_HITREACTION_CRITICAL, "HitReaction.Critical");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_HITREACTION_STUN, "HitReaction.Stun");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_HITREACTION_KNOCKBACK, "HitReaction.Knockback");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_HITREACTION_KNOCKDOWN, "HitReaction.Knockdown");

// ============================================================
// WORLD EVENT TAGS
// ============================================================
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_ENVIRONMENT_DOOR_OPEN, "WorldEvent.Environment.Door.Open");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_ENVIRONMENT_LADDER_ENTER, "WorldEvent.Environment.Ladder.Enter");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_INTERACTION_CHEST_OPEN, "WorldEvent.Interaction.Chest.Open");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_INTERACTION_PICKUP, "WorldEvent.Interaction.Pickup");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_INTERACTION_SCAN_SENSE_START, "WorldEvent.Interaction.Scan.Sense.Start");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_INTERACTION_SCAN_SENSE_ENEMY, "WorldEvent.Interaction.Scan.Sense.Enemy");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_INTERACTION_LADDER_CLIMB, "WorldEvent.Interaction.Ladder.Climb");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_INTERACTION_BOX_OPEN, "WorldEvent.Interaction.Box.Open");
UE_DEFINE_GAMEPLAY_TAG(KRTAG_WORLDEVENT_INTERACTION_SAVE_FIRE, "WorldEvent.Interaction.Save.Fire");
