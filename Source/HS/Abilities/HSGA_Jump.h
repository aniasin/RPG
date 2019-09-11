// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/HSGameplayAbility.h"
#include "HSGA_Jump.generated.h"

/**
 * 
 */
UCLASS()
class HS_API UHSGA_Jump : public UHSGameplayAbility
{
	GENERATED_BODY()

public:
	UHSGA_Jump();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
 		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
};
