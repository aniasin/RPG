// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "PlayerAttributesSet.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UPlayerAttributesSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  AttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  DefenseMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  Stamina;

};
