// SillikOne.


#include "NpcCharacter.h"
#include "AI/Npc_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Abilities/PlayerAttributesSet.h"



ANpcCharacter::ANpcCharacter():Super()
{

}

void ANpcCharacter::BeginPlay()
{
	Super::BeginPlay();

}

bool ANpcCharacter::IsEquipped()
{
	if (!CurrentWeaponLeft && !CurrentWeaponRight)
	{
		return false;
	}
	return true;
}

void ANpcCharacter::SetCombatBehavior()
{
	if (Status != EStatus::InCombat)
	{
		SwitchCombat();
	}

	float CurrentHealth = AttributesComponent->Health.GetCurrentValue();
	float MaxHealth = AttributesComponent->MaxHealth.GetCurrentValue();
	PercentHP = (CurrentHealth / MaxHealth) * 100;

	GameplayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Combat"));
	GameplayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Defend"));
	GameplayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Retreat"));
	GameplayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Flee"));

	if (PercentHP >= 50.f)
	{
		GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Combat"));
		UE_LOG(LogTemp, Warning, TEXT("I AM AGGRESSIVE!"))
	}
	else if (PercentHP >= 25.f)
	{
		GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Defend"));
		UE_LOG(LogTemp, Warning, TEXT("I AM DEFENSIVE!"))
	}
	else if (PercentHP >= 10.f)
	{
		GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Retreat"));
		UE_LOG(LogTemp, Warning, TEXT("I AM RETREATING!"))
	}
	else if (PercentHP < 10.f)
	{
		GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Flee"));
		UE_LOG(LogTemp, Warning, TEXT("I AM FLEEING!"))
	}

}

void ANpcCharacter::SetCanSeePlayer(bool bCanSee)
{
	GameplayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Status.Aggressive.CanSee"));
	GameplayTags.RemoveTag(FGameplayTag::RequestGameplayTag("Status.Aggressive.CannotSee"));
	if (bCanSee)
	{
		GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Status.Aggressive.CanSee"));
	}
	else if (!bCanSee)
	{
		GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Status.Aggressive.CannotSee"));
	}
}
