# RPG
UE4.22 Powered.

### SetUp
+ **Climbing**

Climbable objects have to be WorldStatic.
With overlap response to SphereTracer channel.

### Controls
+ Movement: ZQSD

+ Left Alt: Switch Combat/Peace

+ Space: jump(Peace) or Dash (Combat)

+ Escape: Pause menu

## Features

### COMBAT

+ **Ability System**

Health. 
AttackSpeed.

### UI

+ **Main Menu**

+ **Pause menu**

Settings>>Keybinding.
Quit.
Resume.

### MOVEMENT

+ **Climbing**

Rough system... lot to tweak.
MoveForward to climb(see setup).
Jump to climb ledges (while hanging).

+ **Dash Ability**

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

Simple BTtask: Npc looks for weapons and equips.
Stops looking for weapon when fully equipped.

Simple BTtask: Npc Search Player.
When sight is lost pursue the last player's position seen,
then last direction the player was moving to, and finally a random spot in range.

Simple BTtask: Npc Aggro.
Follow player when in sight.
When Player's reached, NPC attacks, defend, retreat or flee (Only UE_LOG for now).

+ **EnvironmentQuery**

Weapons in range.
