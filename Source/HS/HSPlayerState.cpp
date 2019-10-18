// SillikOne.


#include "HSPlayerState.h"
#include "Abilities/HSAttributeSetBase.h"
#include "Abilities/HSAbilitySystemComponent.h"
#include "CharacterV2.h"
#include "HSPlayerController.h"
#include "UI/HSFloatingStatusBarWidget.h"
#include "UI/HSHUDWidget.h"
#include "AI/Npc_AIController.h"

AHSPlayerState::AHSPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UHSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UHSAttributeSetBase>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
}

UAbilitySystemComponent* AHSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

class UHSAttributeSetBase* AHSPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}

bool AHSPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AHSPlayerState::ShowAbilityConfirmCancelText(bool ShowText)
{
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->ShowAbilityConfirmCancelText(ShowText);
		}
	}
}

float AHSPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AHSPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AHSPlayerState::GetHealthRegenRate() const
{
	return AttributeSetBase->GetHealthRegenRate();
}

float AHSPlayerState::GetMana() const
{
	return AttributeSetBase->GetMana();
}

float AHSPlayerState::GetMaxMana() const
{
	return AttributeSetBase->GetMaxMana();
}

float AHSPlayerState::GetManaRegenRate() const
{
	return AttributeSetBase->GetManaRegenRate();
}

float AHSPlayerState::GetStamina() const
{
	return AttributeSetBase->GetStamina();
}

float AHSPlayerState::GetMaxStamina() const
{
	return AttributeSetBase->GetMaxStamina();
}

float AHSPlayerState::GetStaminaRegenRate() const
{
	return AttributeSetBase->GetStaminaRegenRate();
}

float AHSPlayerState::GetArmor() const
{
	return AttributeSetBase->GetArmor();
}

float AHSPlayerState::GetMoveSpeed() const
{
	return AttributeSetBase->GetMoveSpeed();
}

int32 AHSPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetCharacterLevel();
}

int32 AHSPlayerState::GetXP() const
{
	return AttributeSetBase->GetXP();
}

int32 AHSPlayerState::GetXPBounty() const
{
	return AttributeSetBase->GetXPBounty();
}

int32 AHSPlayerState::GetGold() const
{
	return AttributeSetBase->GetGold();
}

int32 AHSPlayerState::GetGoldBounty() const
{
	return AttributeSetBase->GetGoldBounty();
}

void AHSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AHSPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &AHSPlayerState::MaxHealthChanged);
		HealthRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthRegenRateAttribute()).AddUObject(this, &AHSPlayerState::HealthRegenRateChanged);
		ManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AHSPlayerState::ManaChanged);
		MaxManaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &AHSPlayerState::MaxManaChanged);
		ManaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaRegenRateAttribute()).AddUObject(this, &AHSPlayerState::ManaRegenRateChanged);
		StaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaAttribute()).AddUObject(this, &AHSPlayerState::StaminaChanged);
		MaxStaminaChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetMaxStaminaAttribute()).AddUObject(this, &AHSPlayerState::MaxStaminaChanged);
		StaminaRegenRateChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetStaminaRegenRateAttribute()).AddUObject(this, &AHSPlayerState::StaminaRegenRateChanged);
		XPChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetXPAttribute()).AddUObject(this, &AHSPlayerState::XPChanged);
		GoldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetGoldAttribute()).AddUObject(this, &AHSPlayerState::GoldChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetCharacterLevelAttribute()).AddUObject(this, &AHSPlayerState::CharacterLevelChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AHSPlayerState::StunTagChanged);
	}
}

void AHSPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	ACharacterV2* Hero = Cast<ACharacterV2>(GetPawn());
	if (Hero)
	{
		UHSFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
		}
	}

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint

	// If the player died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (Hero)
		{
			Hero->Die();
		}
	}
}

void AHSPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	float MaxHealth = Data.NewValue;

	// Update floating status bar
	ACharacterV2* Hero = Cast<ACharacterV2>(GetPawn());
	if (Hero)
	{
		UHSFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
		}
	}

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxHealth(MaxHealth);
		}
	}
}

void AHSPlayerState::HealthRegenRateChanged(const FOnAttributeChangeData& Data)
{
	float HealthRegenRate = Data.NewValue;

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetHealthRegenRate(HealthRegenRate);
		}
	}
}

void AHSPlayerState::ManaChanged(const FOnAttributeChangeData& Data)
{
	float Mana = Data.NewValue;

	// Update floating status bar
	ACharacterV2* Hero = Cast<ACharacterV2>(GetPawn());
	if (Hero)
	{
		UHSFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(Mana / GetMaxMana());
		}
	}

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint
}

void AHSPlayerState::MaxManaChanged(const FOnAttributeChangeData& Data)
{
	float MaxMana = Data.NewValue;

	// Update floating status bar
	ACharacterV2* Hero = Cast<ACharacterV2>(GetPawn());
	if (Hero)
	{
		UHSFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetManaPercentage(GetMana() / MaxMana);
		}
	}

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxMana(MaxMana);
		}
	}
}

void AHSPlayerState::ManaRegenRateChanged(const FOnAttributeChangeData& Data)
{
	float ManaRegenRate = Data.NewValue;

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetManaRegenRate(ManaRegenRate);
		}
	}
}

void AHSPlayerState::StaminaChanged(const FOnAttributeChangeData& Data)
{
	float Stamina = Data.NewValue;

	// Update the HUD
	// Can be Handled in the UI itself using the AsyncTaskAttributeChanged node
	// Update floating status bar
	ACharacterV2* Hero = Cast<ACharacterV2>(GetPawn());
	if (Hero)
	{
		UHSFloatingStatusBarWidget* HeroFloatingStatusBar = Hero->GetFloatingStatusBar();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetStaminaPercentage(Stamina /GetMaxStamina());
		}
	}
}

void AHSPlayerState::MaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	float MaxStamina = Data.NewValue;

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetMaxStamina(MaxStamina);
		}
	}
}

void AHSPlayerState::StaminaRegenRateChanged(const FOnAttributeChangeData& Data)
{
	float StaminaRegenRate = Data.NewValue;

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetStaminaRegenRate(StaminaRegenRate);
		}
	}
}

void AHSPlayerState::XPChanged(const FOnAttributeChangeData& Data)
{
	float XP = Data.NewValue;

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetExperience(XP);
		}
	}
}

void AHSPlayerState::GoldChanged(const FOnAttributeChangeData& Data)
{
	float Gold = Data.NewValue;

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetGold(Gold);
		}
	}
}

void AHSPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	float CharacterLevel = Data.NewValue;

	// Update the HUD
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetOwner());
	if (PC)
	{
		UHSHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetHeroLevel(CharacterLevel);
		}
	}
}

void AHSPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}
