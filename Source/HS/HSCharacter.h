// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "HSCharacter.generated.h"

//It's very important that this enum is UENUM, because the code will look for UENUM by the given name and crash if the UENUM can't be found. BlueprintType is there so we can use these in blueprints, too. Just in case. Can be neat to define ability packages.
/*UENUM(BlueprintType)
enum class AbilityInput : uint8
{
	UseAbility1 UMETA(DisplayName = "Use Spell 1"), //This maps the first ability(input ID should be 0 in int) to the action mapping(which you define in the project settings) by the name of "UseAbility1". "Use Spell 1" is the blueprint name of the element.
	DashAbility UMETA(DisplayName = "DashAbility"), //Maps ability 2(input ID 1) to action mapping UseAbility2. "Use Spell 2" is mostly used for when the enum is a blueprint variable.
	UseAbility3 UMETA(DisplayName = "Use Spell 3"),
	WeaponAbility UMETA(DisplayName = "Use Weapon"), //This finally maps the fifth ability(here designated to be your weaponability, or auto-attack, or whatever) to action mapping "WeaponAbility".

		//You may also do something like define an enum element name that is not actually mapped to an input, for example if you have a passive ability that isn't supposed to have an input. This isn't usually necessary though as you usually grant abilities via input ID,
		//which can be negative while enums cannot. In fact, a constant called "INDEX_NONE" exists for the exact purpose of rendering an input as unavailable, and it's simply defined as -1.
		//Because abilities are granted by input ID, which is an int, you may use enum elements to describe the ID anyway however, because enums are fancily dressed up ints.
};*/

UENUM(BlueprintType)
enum class EStatus : uint8
{
	InPeace UMETA(DisplayName = "In Peace"),
	InCombat UMETA(DisplayName = "In Combat")
};

UCLASS(config=Game)
class AHSCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY()
	TArray <class APotion*> EquippedPotions;

	virtual void Jump()override;

	//Basic melee attack
	void MeleeAttack();

	//Ability when using potion
	void UsePotion();

	void SwitchCombat();

public:
	AHSCharacter();
	
	// Player Status ie: Combat, peace, ect.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Status)
	EStatus Status;

	// Implement IAbilitySystemInterface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	void OnHealthChanged(float Health, float MaxHealth);
	UFUNCTION(BlueprintImplementableEvent, Category = CharacterStats, meta = (DisplayName = "OnHealthChanged"))
	void K2_OnHealthChanged(float Health, float MaxHealth);
	UFUNCTION(BlueprintImplementableEvent, Category = CharacterStats, meta = (DisplayName = "OnDeath"))
	void K2_OnDeath();

	// Initialize Abilities and Attributes
	UFUNCTION(BlueprintCallable, Category = Abilities)
		void AquireAbility(TArray <TSubclassOf<UGameplayAbility>>AbilitiesToAdd);

	//Interactions
	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapBegin(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, Category = Collision)
		void OnOverlapEnd(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

	void Interaction();
	bool bHasActorToUse;

	AActor* CurrentFocusedObject;
	void Takeobject(AActor* OtherActor);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	virtual void PostInitializeComponents()override;

protected:
	virtual void BeginPlay()override;
	virtual void PossessedBy(AController* NewController)override;

	/** Our ability system */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	class UDataTable* AttrDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UHS_AbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
	class UPlayerAttributesSet* AttributesComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
	TArray <class TSubclassOf<UGameplayAbility>> Abilities;

	//Movement input
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	bool bIsDead;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
