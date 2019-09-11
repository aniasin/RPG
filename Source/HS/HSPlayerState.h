// SillikOne.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "HSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HS_API AHSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AHSPlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UHSAttributeSetBase* GetAttributeSetBase() const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumentation|GDPlayerState")
		bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "GASDocumenation|GDPlayerState|UI")
		void ShowAbilityConfirmCancelText(bool ShowText);


	/**
	* Getters for attributes from GDAttributeSetBase. Returns Current Value unless otherwise specified.
	*/

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetManaRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetStaminaRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetArmor() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		float GetMoveSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		int32 GetXP() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		int32 GetXPBounty() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		int32 GetGold() const;

	UFUNCTION(BlueprintCallable, Category = "HS Parameters|HSPlayerState|Attributes")
		int32 GetGoldBounty() const;

protected:
	UPROPERTY()
		class UHSAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
		class UHSAttributeSetBase* AttributeSetBase;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle HealthRegenRateChangedDelegateHandle;
	FDelegateHandle ManaChangedDelegateHandle;
	FDelegateHandle MaxManaChangedDelegateHandle;
	FDelegateHandle ManaRegenRateChangedDelegateHandle;
	FDelegateHandle StaminaChangedDelegateHandle;
	FDelegateHandle MaxStaminaChangedDelegateHandle;
	FDelegateHandle StaminaRegenRateChangedDelegateHandle;
	FDelegateHandle XPChangedDelegateHandle;
	FDelegateHandle GoldChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void HealthRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void ManaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxManaChanged(const FOnAttributeChangeData& Data);
	virtual void ManaRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void StaminaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxStaminaChanged(const FOnAttributeChangeData& Data);
	virtual void StaminaRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void XPChanged(const FOnAttributeChangeData& Data);
	virtual void GoldChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
