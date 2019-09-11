// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


UCLASS()
class HS_API AWeapon : public AActor
{
	GENERATED_BODY()
	
	// Sets default values for this actor's properties
	AWeapon();

public:
	// The owner of this
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Equipment)
		AActor* OwnerActor;

	//Item Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		bool bIsLeftHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		float SpeedMultiplier;

	// Animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
		TArray <UAnimMontage*> AnimMontages;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

};
