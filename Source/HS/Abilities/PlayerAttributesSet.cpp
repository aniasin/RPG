// SillikOne.


#include "PlayerAttributesSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"


UPlayerAttributesSet::UPlayerAttributesSet()
{

}


void UPlayerAttributesSet::PostGameplayEffectExecute (const struct FGameplayEffectModCallbackData& Data)
{
	Health.SetCurrentValue(FMath::Clamp(Health.GetCurrentValue(), .0f, MaxHealth.GetCurrentValue()));
	Health.SetBaseValue(FMath::Clamp(Health.GetBaseValue(), .0f, MaxHealth.GetBaseValue()));
	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<UProperty>(UPlayerAttributesSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UPlayerAttributesSet, Health))) 
	{
		OnHealthChanged.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());
	}
}
