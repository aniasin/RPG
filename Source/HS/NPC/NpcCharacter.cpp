// SillikOne.


#include "NpcCharacter.h"
#include "AI/Npc_AIController.h"
#include "BehaviorTree/BehaviorTree.h"



ANpcCharacter::ANpcCharacter():Super()
{

}

bool ANpcCharacter::IsEquipped()
{
	if (!CurrentWeaponLeft && !CurrentWeaponRight)
	{
		return false;
	}
	return true;
}

void ANpcCharacter::BeginPlay()
{
	Super::BeginPlay();
}

