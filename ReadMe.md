# RPG
UE4.22 Powered.

### SetUp
Drop those directories from Thirdperson Template in Content/
+ /Geometry

+ /ThirdPerson

+ Climbing
Climbable objects have to be WorldStatic
With overlap response to SphereTracer channel

### Controls
+ Movement: ZQSD

+ Ability1: Numpad 1 (only printString)

+ See PlayerStats: Numpad +

+ Left Alt: Switch Combat/Peace

+ Space: jump(Peace) or Dash (Combat)

+ Escape: Pause menu

### Features
+ Ability System
Health
AttackSpeed

+ Pause menu
Settings>>Keybinding
Quit
Resume

+ Climbing
Rough system... lot to tweak
MoveForward to climb(see setup)
Jump to climb ledges (while hanging)

+ Configurable potions
EffectAmount (Positive/Negative)
Can be Overtime with duration or instant

+ Dynamic Material
Red potions with negative effect, Green positive.
