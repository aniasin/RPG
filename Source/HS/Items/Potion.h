// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Potion.generated.h"

UCLASS()
class HS_API APotion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APotion();

	float GetEffectAmount();
	bool GetIsOvertime();
	float GetDuration();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Effect Amount, can be positive or negative.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	float Amount;
	//Effect has duration?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	bool bEffectOverTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	float Duration;;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		bool bIsEmpty;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
