// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HSAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UHSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	APawn* OwningPawn;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;

	UFUNCTION(BlueprintCallable, Category = "AnimNotify")
		void Notify_SwitchWeapon();
};
