// SillikOne.


#include "PlayerAttributesSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"


UPlayerAttributesSet::UPlayerAttributesSet()
{

}


void UPlayerAttributesSet::PostGameplayEffectExecute (const struct FGameplayEffectModCallbackData& Data)
{
	if (Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<UProperty>(UPlayerAttributesSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UPlayerAttributesSet, Health))) 
	{
		OnHealthChanged.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());
	}
}
