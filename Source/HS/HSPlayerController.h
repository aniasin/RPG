// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HS_API AHSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = UserInterface)
		void ToggleInteractionWidget();
};
