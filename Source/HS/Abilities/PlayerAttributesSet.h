// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "PlayerAttributesSet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedDel, float, Health, float, MaxHealth);
/**
 * 
 */
UCLASS()
class HS_API UPlayerAttributesSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPlayerAttributesSet();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
		FGameplayAttributeData MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  AttackMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  DefenseMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FGameplayAttributeData  Stamina;

	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)override;
	FOnHealthChangedDel OnHealthChanged;

};
