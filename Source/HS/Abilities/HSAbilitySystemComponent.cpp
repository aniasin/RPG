// SillikOne.


#include "HSAbilitySystemComponent.h"

void UHSAbilitySystemComponent::ReceiveDamage(UHSAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
	ReceivedDamage.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}
