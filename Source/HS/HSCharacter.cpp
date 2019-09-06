// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/HS_AbilitySystemComponent.h"
#include "Abilities/PlayerAttributesSet.h"
#include "Items/Potion.h"
#include "Items/Weapon.h"
#include "Classes/Components/SceneComponent.h"
#include "Classes/GameFramework/Controller.h"
#include "HSPlayerController.h"


//////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR ----------------------------------------------------
AHSCharacter::AHSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UHS_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AttributesComponent = CreateDefaultSubobject<UPlayerAttributesSet>(TEXT("AttributesComponent"));

	bIsDead = false;
}

void AHSCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTags;
	return;
}

UAbilitySystemComponent* AHSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

//////////////////////////////////////////////////////////////////////////
// INPUT --------------------------------------------------------------
void AHSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AHSCharacter::RightHand);
	PlayerInputComponent->BindAction("LeftHand", IE_Pressed, this, &AHSCharacter::LeftHand);

	//Items on belt
	PlayerInputComponent->BindAction("Belt_01", IE_Pressed, this, &AHSCharacter::UsePotion);
	//Interaction toggle
	PlayerInputComponent->BindAction("Interaction", IE_Pressed, this, &AHSCharacter::Interaction);
	//PlayerInputComponent->BindAction("Interaction", IE_Released, this, &AHSCharacter::ToggleInteraction);
	PlayerInputComponent->BindAction("SwitchCombat", IE_Pressed, this, &AHSCharacter::SwitchCombat);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHSCharacter::LookUpAtRate);

	//AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
}

inline UGameplayEffect* ConstructGameplayEffect(FString name)
{
	return NewObject<UGameplayEffect>(GetTransientPackage(), FName(*name));
}

inline FGameplayModifierInfo& AddModifier(
	UGameplayEffect* Effect, UProperty* Property,
	EGameplayModOp::Type Op,
	const FGameplayEffectModifierMagnitude& Magnitude)
{
	int32 index = Effect->Modifiers.Num();
	Effect->Modifiers.SetNum(index + 1);
	FGameplayModifierInfo& Info = Effect->Modifiers[index];
	Info.ModifierMagnitude = Magnitude;
	Info.ModifierOp = Op;
	Info.Attribute.SetUProperty(Property);
	return Info;
}
/////////////////////////////
// BEGINPLAY ---------------
void AHSCharacter::BeginPlay()
{
	Super::BeginPlay();

	AquireAbility(Abilities);
	//Subscribe to OnHealthChanged delegate from AttributeSet
	AttributesComponent->OnHealthChanged.AddDynamic(this, &AHSCharacter::OnHealthChanged);
	
}

void AHSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (RootComponent)
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHSCharacter::OnOverlapBegin);
		GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AHSCharacter::OnOverlapEnd);
	}
}

void AHSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystemComponent->RefreshAbilityActorInfo();
}

///////////////////////////////////
// ABILITIES --------------------
void AHSCharacter::AquireAbility(TArray <TSubclassOf<UGameplayAbility>>AbilitiesToAdd)
{
	if (AbilitySystemComponent && Abilities.Num() > 0)
	{
		//Initialize Abilities
		if (HasAuthority())
		{
			for (int32 i=0; i < Abilities.Num(); i++)
			{
				auto AbilityToAquire = Abilities[i];
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityToAquire.GetDefaultObject(), 1, i));
				AbilitySystemComponent->InitAbilityActorInfo(this, this);
			}
		}
		//Initialize Attributes
		if (AbilitySystemComponent && AttrDataTable)
		{
			const UAttributeSet* Attrs = AbilitySystemComponent->InitStats(UPlayerAttributesSet::StaticClass(), AttrDataTable);
		}
	}
}

/////////////////////////////
//ATTRIBUTES -----------------
void AHSCharacter::OnHealthChanged(float Health, float MaxHealth)
{
	if (Health <= .0f && !bIsDead)
	{
		bIsDead = true;
		K2_OnDeath();
	}
	K2_OnHealthChanged(Health, MaxHealth);
}



//////////////////////////////
// MOVEMENTS ----------------
void AHSCharacter::Jump()
{
	//Cannot jump while engaged in combat..
	if (Status == EStatus::InPeace)
	{
		Super::Jump();
	}
	else
	{
		//.. but Dash
		AbilitySystemComponent->TryActivateAbilityByClass(Abilities[0]);
		FVector Direction = GetActorForwardVector(); //TODO change direction depending on input(right, left, front or back)
		this->LaunchCharacter((Direction * 5000.f), false, false);//TODO perhaps could be variable (depending on skill)
	}
}

void AHSCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AHSCharacter::MoveForward(float Value)
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

void AHSCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
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

//////////////////////////////////////
// COMBAT ---------------------------
void AHSCharacter::SwitchCombat()
{
	//Change status for AnimBP to know and attach weapons (if there is) on correct socket
	if (Status == EStatus::InPeace)
	{
		Status = EStatus::InCombat;
		bUseControllerRotationYaw = true;
		if (CurrentWeaponRight)
		{
			CurrentWeaponRight->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_r"));
		}
		if (CurrentWeaponLeft)
		{
			CurrentWeaponLeft->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_l"));
		}
	}
	else
	{
		Status = EStatus::InPeace;
		bUseControllerRotationYaw = false;
		if (CurrentWeaponRight)
		{
			CurrentWeaponRight->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponBack"));
		}
		if (CurrentWeaponLeft)
		{
			CurrentWeaponLeft->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponBack2"));
		}
	}
}

void AHSCharacter::RightHand()
{
	if (Status == EStatus::InCombat && CurrentWeaponRight)
	{
		//Basic Melee attack
		AbilitySystemComponent->TryActivateAbilityByClass(Abilities[1]);
	}
}

void AHSCharacter::LeftHand()
{
	if (Status == EStatus::InCombat && CurrentWeaponLeft)
	{
		AbilitySystemComponent->TryActivateAbilityByClass(Abilities[2]);
	}
}

////////////////////////////////
// INTERACTIONS -----------------
//Setup
void AHSCharacter::OnOverlapBegin_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	auto PlayerController = Cast<AHSPlayerController>(GetWorld()->GetFirstPlayerController());
	auto CurrentController = GetController();

	// Interaction is only possible with actor tagged for
	if (OtherActor->ActorHasTag(TEXT("Interact")))
	{
		if (CurrentController == PlayerController) //Only toggle widget if it's the player
		{
			PlayerController->ToggleInteractionWidget();
		}
		CurrentFocusedObject = OtherActor;
		bHasActorToUse = true; //There is something to interact with
	}
}

void AHSCharacter::OnOverlapEnd_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto PlayerController = Cast<AHSPlayerController>(GetWorld()->GetFirstPlayerController());
	auto CurrentController = GetController();

	// Interaction is only possible with actor tagged for
	if (OtherActor->ActorHasTag(TEXT("Interact")))
	{
		if (CurrentController == PlayerController) //Only toggle widget if it's the player
		{
			PlayerController->ToggleInteractionWidget();
		}
	}
	if (OtherActor == CurrentFocusedObject) 
	{
		bHasActorToUse = false; //There is nothing to interact with
	}
}

void AHSCharacter::Interaction()
{
	//If there is something to use and player hit "Interaction" input
	if (bHasActorToUse)
	{
		TakeObject(CurrentFocusedObject);
	}
}

void AHSCharacter::TakeObject(AActor* OtherActor)
{
	// Potion
	APotion* Potion = Cast<APotion>(OtherActor);
	if (Potion)//TODO Make enum?
	{
		// Get current Equipped potions count
		int32 PotionCount = EquippedPotions.Num();
		// if potions count is 5, belt is full
		if (PotionCount > 4) { return; } //TODO offer opportunity to replace current equipped potions

		//Get Potion and attach to belt at the correct socket
		const FString Socket = FString::Printf(TEXT("Potion_%d"), PotionCount);
		Potion->SetActorEnableCollision(false);
		Potion->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, (TEXT("%s"), *Socket));
		//Add Potion to EquipedPotions Array
		EquippedPotions.Add(Potion);
	}
	// Weapon
	AWeapon* Weapon = Cast<AWeapon>(OtherActor);
	if (Weapon)
	{
		// take the weapon and register it as currently equipped
		if (Weapon->bIsLeftHand && !CurrentWeaponLeft) //TODO offer opportunity to change weapon if player has already one
		{
			K2_TakeWeapon(Weapon);
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponBack2"));
			Weapon->Tags.RemoveAt(0);
			Weapon->OwnerActor = this;
			CurrentWeaponLeft = Weapon;
			GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Equipment.Left"));
		}
		else if (!(Weapon->bIsLeftHand) && !CurrentWeaponRight)
		{
			K2_TakeWeapon(Weapon);
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponBack"));
			Weapon->Tags.RemoveAt(0);
			Weapon->OwnerActor = this;
			CurrentWeaponRight = Weapon;
			GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Equipment.Right"));
			
		}
	}
}

void AHSCharacter::UsePotion()
{
	//Check Equipped Potions if there is to use
	if (EquippedPotions.Num() > 0)
	{
		APotion* Potion = EquippedPotions[0]; //TODO Map 1-5, so player can choose which to use.
				
		// Get Potion Properties
		float EffectAmount = Potion->GetEffectAmount();
		bool bIsOvertime = Potion->GetIsOvertime();
		float Duration = Potion->GetDuration();

		//if full health and it's not poison, don't use potion //TODO Player feedback
		float MaxHealth = AttributesComponent->MaxHealth.GetCurrentValue();
		float CurrentHealth = AttributesComponent->Health.GetCurrentValue();
		if (CurrentHealth >= MaxHealth && EffectAmount > 0.1f) {return;}

		// Construct & retrieve UProperty to affect
		UGameplayEffect* RecoverHP = ConstructGameplayEffect("RecoverHP");

		UProperty* hpProperty = FindFieldChecked<UProperty>(
			UPlayerAttributesSet::StaticClass(),
			GET_MEMBER_NAME_CHECKED(UPlayerAttributesSet, Health));

		// Command the addition of EffectAmount HP to the hpProperty
		AddModifier(RecoverHP, hpProperty, EGameplayModOp::Additive, FScalableFloat(EffectAmount));
		// .. overtime effect if checked
		if (bIsOvertime)
		{
			RecoverHP->DurationPolicy = EGameplayEffectDurationType::HasDuration;
			RecoverHP->DurationMagnitude = FScalableFloat(Duration);
		}
		// .. with 100% chance of success ..
		RecoverHP->ChanceToApplyToTarget = 1.f;
		// .. every second
		RecoverHP->Period = 1.0f;
		FActiveGameplayEffectHandle recoverHpEffectHandle = AbilitySystemComponent->ApplyGameplayEffectToTarget(
			RecoverHP, AbilitySystemComponent, 1.f);
		//Then Destroy potion and remove from Array
		Potion->Destroy();
		EquippedPotions.RemoveAt(0);
	}
}

