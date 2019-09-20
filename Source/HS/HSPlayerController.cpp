// SillikOne.


#include "HSPlayerController.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/HSDamageTextWidgetComponent.h"
#include "HSPlayerState.h"
#include "UI/HSHUDWidget.h"
#include "GenericTeamAgentInterface.h"

AHSPlayerController::AHSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	SetGenericTeamId(1);
}

void AHSPlayerController::CreateHUD()
{
	// Only create once
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), TEXT(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}

	// Need a valid PlayerState to get attributes from
	AHSPlayerState* PS = GetPlayerState<AHSPlayerState>();
	if (!PS)
	{
		return;
	}

	UIHUDWidget = CreateWidget<UHSHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	// Set attributes
	UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / PS->GetMaxHealth());
	UIHUDWidget->SetCurrentMana(PS->GetMana());
	UIHUDWidget->SetMaxMana(PS->GetMaxMana());
	UIHUDWidget->SetManaPercentage(PS->GetMana() / PS->GetMaxMana());
	UIHUDWidget->SetHealthRegenRate(PS->GetHealthRegenRate());
	UIHUDWidget->SetManaRegenRate(PS->GetManaRegenRate());
	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / PS->GetMaxStamina());
	UIHUDWidget->SetStaminaRegenRate(PS->GetStaminaRegenRate());
	UIHUDWidget->SetExperience(PS->GetXP());
	UIHUDWidget->SetGold(PS->GetGold());
	UIHUDWidget->SetHeroLevel(PS->GetCharacterLevel());

	DamageNumberClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/UI/Characters/W_HSDamageTextWidgetComponent.W_HSDamageTextWidgetComponent_C"));
	if (!DamageNumberClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find DamageNumberClass. If it was moved, please update the reference location in C++."), TEXT(__FUNCTION__));
	}
}

class UHSHUDWidget* AHSPlayerController::GetHUD()
{
	return UIHUDWidget;
}

void AHSPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AHSCharacterBase* TargetCharacter)
{
	UHSDamageTextWidgetComponent* DamageText = NewObject<UHSDamageTextWidgetComponent>(TargetCharacter, DamageNumberClass);
	DamageText->RegisterComponent();
	DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DamageText->SetDamageText(DamageAmount);
}

bool AHSPlayerController::ShowDamageNumber_Validate(float DamageAmount, AHSCharacterBase* TargetCharacter)
{
	return true;
}

void AHSPlayerController::SetRespawnCountdown_Implementation(float RespawnTimeRemaining)
{
	if (UIHUDWidget)
	{
		UIHUDWidget->SetRespawnCountdown(RespawnTimeRemaining);
	}
}

bool AHSPlayerController::SetRespawnCountdown_Validate(float RespawnTimeRemaining)
{
	return true;
}

void AHSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AHSPlayerState* PS = GetPlayerState<AHSPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

