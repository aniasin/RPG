# RPG
UE4.23 Powered.

Using [tranek's GASDocumentation](https://github.com/tranek/GASDocumentation) wonderfull sample.

Using [Advanced Cel Shader Lite](https://www.unrealengine.com/marketplace/en-US/slug/ce2cda95e5f54180b2cf7df24566bdf7)

Last Packaged Build: [2019-11-1](https://drive.google.com/file/d/1K2HB5MCvXifJeS1wVnijRYKm-8wHUqtb/view?usp=sharing)

### SetUp

### Controls
+ **Movement:** Keyboard + Mouse ZQSD or/and Gamepad(both configurable in EscapeMenu)
+ **PauseMenu:** Escape / Gamepad Left Special

## Features

### MULTIPLAYER

### DIALOG SYSTEM
Dynamic system that allow npc to share informations(rough).



### COMBAT

### UI

+ **Loading Screen**

+ **Main Menu**
Host, Join(IP), Quit

+ **Pause menu**
Settings>>Keybinding 
Quit.
Resume.

### MOVEMENT

+ **Climbing** (To be Refactored)

Rough system... lot to tweak.
MoveForward to climb(see setup).
Jump to climb ledges (while hanging).

+ **Dash Ability**

Dash instead of Jump while in combat.

### ITEMS

+ **Configurable potions**

EffectAmount (Positive/Negative).
Can be Overtime with duration or instant.

+ **Weapons**

Sword, Shield, Great Sword, Bow

+ **Dynamic Material**

Red potions with negative effect, Green positive.(To be erefactored)

### AI

+ **AiSight and Hear**

NPCs detect player entering or exiting sight.

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
