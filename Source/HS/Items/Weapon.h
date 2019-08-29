// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class HS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Item Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool bIsLeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float SpeedMultiplier;

private:

};
