# RPG
UE4.23 Powered.
Using [tranek's GASDocumentation](https://github.com/tranek/GASDocumentation) wonderfull sample.

### SetUp


### Controls
+ Movement: ZQSD

+ Left Alt: Switch Combat/Peace

+ Space: jump while in peace or Dash while in combat (to be refactored)

+ Escape: Pause menu (To be refactored)

## Features

### MULTIPLAYER

### COMBAT

### UI (To be Refactored)

+ **Main Menu**

+ **Pause menu**

Settings>>Keybinding.
Quit.
Resume.

### MOVEMENT

+ **Climbing** (To be Refactored)

Rough system... lot to tweak.
MoveForward to climb(see setup).
Jump to climb ledges (while hanging).

+ **Dash Ability** (To be Refactored)

Dash forward while in combat.

### ITEMS

+ **Configurable potions**

EffectAmount (Positive/Negative).
Can be Overtime with duration or instant.

+ **Configurable Weapons**

Damage, SpeedMultiplier, left or right hand.

+ **Dynamic Material**

Red potions with negative effect, Green positive.

### AI

+ **AiSight and Hear**

NPCs detect player entering or exitind sight.

+ **BehaviorTree**

**SearchWeapons**: Npc looks for weapons and equips.
Stops looking for weapon when fully equipped.

**SearchPlayer**: Npc Search Player.
When sight is lost pursue the last player's position seen,
then last direction the player was moving to, and finally a random spot in range.

**Patrol**: NPC patrol when not in alert

**Combat**: NPC Attacks, Defends, retreats or flees. (To be Refactored)

+ **EnvironmentQuery**

Weapons in range.
