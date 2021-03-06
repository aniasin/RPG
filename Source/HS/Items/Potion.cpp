// SillikOne.


#include "Potion.h"
#include "Classes/Components/CapsuleComponent.h"
#include "GameplayEffect.h"
#include "Abilities/HSAttributeSetBase.h"
#include "Abilities/HSAbilitySystemComponent.h"
#include "CharacterV2.h"

// Sets default values
APotion::APotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

float APotion::GetEffectAmount() { return Amount; }
bool APotion::GetIsOvertime() { return bIsOverTime; }
float APotion::GetDuration() { return Duration; }

// Called when the game starts or when spawned
void APotion::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

