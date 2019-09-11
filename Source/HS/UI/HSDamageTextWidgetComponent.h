// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HSDamageTextWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UHSDamageTextWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetDamageText(float Damage);

};
