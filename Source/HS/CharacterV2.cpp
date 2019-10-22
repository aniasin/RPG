// SillikOne.


#include "CharacterV2.h"
#include "HS.h"
#include "UnrealNetwork.h"
#include "Abilities/HSAttributeSetBase.h"
#include "AI/Npc_AIController.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HSGameMode.h"
#include "Abilities/HSAbilitySystemComponent.h"
#include "HSPlayerController.h"
#include "HSPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HSFloatingStatusBarWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "WidgetComponent.h"
#include "Items/Weapon.h"
#include "EngineUtils.h"
#include "Classes/Engine/Engine.h"
#include "CoreUObject/Public/UObject/UObjectGlobals.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "HSCharacterMovementComponent.h"
#include "Engine/World.h"


ACharacterV2::ACharacterV2(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetRelativeLocation(FVector(0, 0, 68.492264));

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->FieldOfView = 80.0f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	// Makes sure that the animations play on the Server so that we can use bone and socket transforms
	// to do things like spawning projectiles and other FX.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(FName("NoCollision"));

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

	UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/UI/Characters/W_HealthBar.W_HealthBar_C"));
	if (!UIFloatingStatusBarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), TEXT(__FUNCTION__));
	}

	AIControllerClass = ANpc_AIController::StaticClass();

	DeadTag = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

	bReplicates = true;
	bReplicateMovement = true;
}

void ACharacterV2::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterV2, WeaponR);
	DOREPLIFETIME(ACharacterV2, MontageRightHand);
	DOREPLIFETIME(ACharacterV2, WeaponL);
	DOREPLIFETIME(ACharacterV2, MontageLeftHand);
}

// Called to bind functionality to input
void ACharacterV2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (!PlayerInputComponent) { return; }

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterV2::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterV2::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	PlayerInputComponent->BindAxis("LookUp", this, &ACharacterV2::LookUp);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterV2::LookUpRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterV2::TurnRate);

	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &ACharacterV2::Interaction);
	PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &ACharacterV2::DropEquipment);
	PlayerInputComponent->BindAction("SwitchCombat", IE_Pressed, this, &ACharacterV2::SwitchCombat);

	// Bind to AbilitySystemComponent
	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
	FString("CancelTarget"), FString("EGDAbilityInputID"), static_cast<int32>(EGDAbilityInputID::Confirm), static_cast<int32>(EGDAbilityInputID::Cancel)));
}

//Server Only

void ACharacterV2::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AHSPlayerState* PS = GetPlayerState<AHSPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UHSAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		AddStartupEffects();

		AddCharacterAbilities();

		AHSPlayerController* PC = Cast<AHSPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
			InitializeFloatingStatusBar();
		}

		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
	}
}

class USpringArmComponent* ACharacterV2::GetCameraBoom()
{
	return CameraBoom;
}

class UCameraComponent* ACharacterV2::GetFollowCamera()
{
	return FollowCamera;
}

float ACharacterV2::GetStartingCameraBoomArmLength()
{
	return StartingCameraBoomArmLength;
}

FVector ACharacterV2::GetStartingCameraBoomLocation()
{
	return StartingCameraBoomLocation;
}

class UHSFloatingStatusBarWidget* ACharacterV2::GetFloatingStatusBar()
{
	return UIFloatingStatusBar;
}

void ACharacterV2::FinishDying()
{
	if (Role == ROLE_Authority)
	{
		AHSGameMode* GM = Cast<AHSGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			GM->HeroDied(GetController());
		}
	}

	Super::FinishDying();
}

//////////////////////////////////////
// Item Interactions

void ACharacterV2::Interaction()
{
	if (!CurrentFocusedItem) { return; }

	if (Role < ROLE_Authority)
	{
		ServerTakeItem(CurrentFocusedItem);
	}
	else
	{
		TakeItem(CurrentFocusedItem);
	}
}

void ACharacterV2::ToggleInteractionWidget_Implementation(AActor* Item)
{

	if (!InteractionWidget) { return; }
	if (InteractionWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		K2_ToggleWidget(false);
		CurrentFocusedItem = NULL;
	}
	else
	{
		K2_ToggleWidget(true);
		CurrentFocusedItem = Item;
	}

}

void ACharacterV2::ServerTakeItem_Implementation(AActor* ItemToTake)
{
	AWeapon* IsWeapon = Cast<AWeapon>(ItemToTake);
	if (IsWeapon)
	{
		if (MontagePickUp) { PlayAnimMontage(MontagePickUp); }

		FName Socket = IsWeapon->bIsLeftHand ? FName("WeaponBack2") : FName("WeaponBack");
		IsWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
		IsWeapon->OwnerActor = this;
		IsWeapon->ItemTaken();

		if (IsWeapon->bIsLeftHand)	
		{
			WeaponL = IsWeapon;
			MontageLeftHand = WeaponL->MontageLeftHand;
		} 
		else	
		{
			WeaponR = IsWeapon;
			MontageRightHand = WeaponR->MontageRightHand;
		}
	}
}

bool ACharacterV2::ServerTakeItem_Validate(AActor* ItemToTake)
{
	return true;
}

void ACharacterV2::TakeItem_Implementation(AActor* ItemToTake)
{
	AWeapon* IsWeapon = Cast<AWeapon>(ItemToTake);
	if (IsWeapon)
	{
		if (MontagePickUp)	{ PlayAnimMontage(MontagePickUp); }

		FName Socket = IsWeapon->bIsLeftHand ? FName("WeaponBack2") : FName("WeaponBack");
		IsWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
		IsWeapon->OwnerActor = this;
		IsWeapon->ItemTaken();

		if (IsWeapon->bIsLeftHand)
		{
			WeaponL = IsWeapon;
			MontageLeftHand = WeaponL->MontageLeftHand;
		}
		else
		{
			WeaponR = IsWeapon;
			MontageRightHand = WeaponR->MontageRightHand;
		}
	}
}

void ACharacterV2::DropEquipment()
{
	if (WeaponL)
	{
		if (Role == ROLE_Authority)
		{
			DropItem(WeaponL);
		}
		else
		{
			ServerDropItem(WeaponL);
		}
		WeaponL = NULL;
		MontageLeftHand = NULL;
	}
	if (WeaponR)
	{
		if (Role == ROLE_Authority)
		{
			DropItem(WeaponR);
		}
		else
		{
			ServerDropItem(WeaponR);
		}
		WeaponR = NULL;
		MontageRightHand = NULL;
	}
}

void ACharacterV2::ServerDropItem_Implementation(AActor* ItemToDrop)
{
	AWeapon* Weapon = Cast<AWeapon>(ItemToDrop);
	if (!Weapon) { return; }
	Weapon->Destroy();
}

bool ACharacterV2::ServerDropItem_Validate(AActor* ItemToDrop)
{
	return true;
}


void ACharacterV2::DropItem_Implementation(AActor* ItemToDrop)
{
	AWeapon* Weapon = Cast<AWeapon>(ItemToDrop);
	if (!Weapon) { return; }
	Weapon->Destroy();
}

float ACharacterV2::GetWeaponSpeed()
{
	if (!WeaponR) { return 0; }
	return WeaponR->SpeedMultiplier;
}

/////////////////////////////////////
// Combat
void ACharacterV2::AttachDetachWeaponR_Implementation(bool bIsAttaching)
{
	if (!WeaponR) { return; }
	FName Socket = bIsAttaching ? FName("Weapon_r") : FName("WeaponBack");
	WeaponR->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
}

void ACharacterV2::AttachDetachWeaponL_Implementation(bool bIsAttaching)
{
	if (!WeaponL) { return; }
	FName Socket = bIsAttaching ? FName("Weapon_l") : FName("WeaponBack2");
	WeaponL->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
}


void ACharacterV2::SwitchCombat()
{
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetController());
	if (PC)
	{
		bIsInCombat = !bIsInCombat;
	}
	// If not player, then it's an AI
	else
	{
		K2_AISwitchCombat();
	}
	if (WeaponL->MontageSwitch)	
	{
		PlayAnimMontage(WeaponL->MontageSwitch);
	}
	else if (WeaponR->MontageSwitch)
	{
		PlayAnimMontage(WeaponR->MontageSwitch);
	}
}

void ACharacterV2::IsShieldUp_Implementation(bool bIsShieldUp)
{
	if (!WeaponL) { return; }
	if (bIsShieldUp)
	{
		WeaponL->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		WeaponL->CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ACharacterV2::AIPerformMeleeAttack()
{
	K2_AIPerformMeleeAttack();
}

void ACharacterV2::AIPerformShieldUp()
{
	K2_AIPerformShieldUp();
}

void ACharacterV2::AIPerformDash()
{
	K2_AIPerformDash();
}

void ACharacterV2::MultiCast_UpdateCharacterRotation_Implementation(FRotator Rotation)
{
	SetActorRotation(Rotation);
}

bool ACharacterV2::MultiCast_UpdateCharacterRotation_Validate(FRotator Rotation)
{
	return true;
}

void ACharacterV2::Server_UpdateCharacterRotation_Implementation(FRotator Rotation)
{
	if (!IsLocallyControlled())
	{
		MultiCast_UpdateCharacterRotation(Rotation);
	}
}

bool ACharacterV2::Server_UpdateCharacterRotation_Validate(FRotator Rotation)
{
	return true;
}

/**
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
* So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
*/

void ACharacterV2::BeginPlay()
{
	Super::BeginPlay();

	// Only needed for Heroes placed in world and when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	AHSPlayerController* PC = Cast<AHSPlayerController>(GetController());
	if (PC)
	{
		InitializeFloatingStatusBar();
	}

	StartingCameraBoomArmLength = CameraBoom->TargetArmLength;
	StartingCameraBoomLocation = CameraBoom->RelativeLocation;
}

void ACharacterV2::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsInCombat)
	{
		if (ROLE_Authority)
		{
			CurrentRotation = GetActorRotation();
			Server_UpdateCharacterRotation(CurrentRotation);
		}
		if (!IsLocallyControlled())
		{
			SetActorRotation(CurrentRotation);
		}
		bUseControllerRotationYaw = true;
	}
	else
	{
		bUseControllerRotationYaw = false;
	}
}

////////////////////////////
// Movement
void ACharacterV2::LookUp(float Value)
{
	if (IsAlive())
	{
		AddControllerPitchInput(Value);
	}
}

void ACharacterV2::LookUpRate(float Value)
{
	if (IsAlive())
	{
		// calculate delta for this frame from the rate information
		AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACharacterV2::Turn(float Value)
{
	if (IsAlive())
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACharacterV2::TurnRate(float Value)
{
	if (IsAlive())
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void ACharacterV2::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterV2::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACharacterV2::InitializeFloatingStatusBar()
{
	// Only create once
	if (UIFloatingStatusBar || !AbilitySystemComponent.IsValid())
	{
		return;
	}

	// Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	AHSPlayerController* PC = Cast<AHSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->IsLocalPlayerController())
	{
		if (UIFloatingStatusBarClass)
		{
			UIFloatingStatusBar = CreateWidget<UHSFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
			if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
			{
				UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

				// Setup the floating status bar
				UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
				UIFloatingStatusBar->SetManaPercentage(GetMana() / GetMaxMana());
				UIFloatingStatusBar->SetStaminaPercentage(GetStamina() / GetMaxStamina());
			}
		}
	}
}

//Client Only

void ACharacterV2::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AHSPlayerState* PS = GetPlayerState<AHSPlayerState>();
	if (PS)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystemComponent = Cast<UHSAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Refresh ASC Actor Info for clients. Server will be refreshed by its AI/PlayerController when it possesses a new Actor.
		AbilitySystemComponent->RefreshAbilityActorInfo();

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		AHSPlayerController* PC = Cast<AHSPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}

		// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
		InitializeFloatingStatusBar();


		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
	}
}
