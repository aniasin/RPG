// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "HSCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/HS_AbilitySystemComponent.h"
#include "Abilities/PlayerAttributesSet.h"
#include "Items/Potion.h"

//////////////////////////////////////////////////////////////////////////
// AHSCharacter

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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

UAbilitySystemComponent* AHSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AHSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHSCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AHSCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AHSCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AHSCharacter::OnResetVR);

	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, FGameplayAbilityInputBinds("ConfirmInput", "CancelInput", "AbilityInput"));
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

void AHSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (AbilitySystemComponent)
	{
		//Initialize Abilities
		if (HasAuthority() && Ability1)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability1.GetDefaultObject(), 1, 0));
		}
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
	//Initialize Attributes
	if (AbilitySystemComponent && AttrDataTable) {
		const UAttributeSet* Attrs = AbilitySystemComponent->InitStats(UPlayerAttributesSet::StaticClass(), AttrDataTable);
	}
}

void AHSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->RefreshAbilityActorInfo();
}

void AHSCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AHSCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AHSCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
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

void AHSCharacter::OnOverlapBegin_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	APotion* IsPotion = Cast<APotion>(OtherActor);
	if (IsPotion)
	{
		UE_LOG(LogTemp, Warning, TEXT("Colliding with %s"), *(OtherActor->GetName()))
		//Get Potion properties and use
		float EffectAmount = IsPotion->GetEffectAmount();
		bool bIsOverTime = IsPotion->GetIsOvertime();
		float Duration = IsPotion->GetDuration();
		UsePotion(EffectAmount, bIsOverTime, Duration);
		IsPotion->Destroy();
	}
}

void AHSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (RootComponent)
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHSCharacter::OnOverlapBegin);
	}
}

void AHSCharacter::UsePotion(float EffectAmount, bool bIsOverTime, float Duration)
{
	// Construct & retrieve UProperty to affect
	UGameplayEffect* RecoverHP = ConstructGameplayEffect("RecoverHP");

	// Compile-time checked retrieval of Hp UPROPERTY()
	// from our UGameUnitAttributeSet class (listed in
	// UGameUnitAttributeSet.h)
	UProperty* hpProperty = FindFieldChecked<UProperty>(
		UPlayerAttributesSet::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UPlayerAttributesSet, Health));

	// Command the addition of +5 HP to the hpProperty
	AddModifier(RecoverHP, hpProperty, EGameplayModOp::Additive, FScalableFloat(EffectAmount));
	// .. for a fixed-duration if checked
	if (bIsOverTime)
	{
		RecoverHP->DurationPolicy = EGameplayEffectDurationType::HasDuration;
		RecoverHP->DurationMagnitude = FScalableFloat(Duration);
	}

	// .. with 100% chance of success ..
	RecoverHP->ChanceToApplyToTarget = 1.f;

	// .. with recurrency (Period) of 1 seconds
	RecoverHP->Period = 1.0f;
	FActiveGameplayEffectHandle recoverHpEffectHandle =	AbilitySystemComponent->ApplyGameplayEffectToTarget(
		RecoverHP, AbilitySystemComponent, 1.f);
}

