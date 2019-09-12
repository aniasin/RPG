// SillikOne.


#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "CharacterV2.h"
#include "Classes/Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComponent;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	CapsuleComponent->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);

	TextRender = CreateDefaultSubobject<UTextRenderComponent>("TextRender");
	TextRender->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	TextRender->SetVisibility(false);
	TextRender->SetText(ItemName);	
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentFocusedActor && HasAuthority())
	{
		UpdateRenderTextRotation(CurrentFocusedActor);
	}
	
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnOverlapBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnOverlapEnd);
}

void AWeapon::OnOverlapBegin_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (!HasAuthority()) {return;}

	CurrentFocusedActor = Cast<ACharacterV2>(OtherActor);
	if (CurrentFocusedActor)
	{
		TextRender->SetVisibility(true);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("OverlapBegin with %s!"),
		*(OtherActor->GetName())));
		UE_LOG(LogTemp, Warning, TEXT("Overlap Begin with %s!"), *OtherActor->GetName())
	}
}

void AWeapon::OnOverlapEnd_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if (!HasAuthority()) {return;}

	if (CurrentFocusedActor)
	{
		TextRender->SetVisibility(false);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("OverlapEnd with %s!"),
		*(OtherActor->GetName())));
		UE_LOG(LogTemp, Warning, TEXT("Overlap End with %s!"), *OtherActor->GetName())
		CurrentFocusedActor = NULL;
	}
}

void AWeapon::UpdateRenderTextRotation(ACharacterV2* CurrentCharacter)
{
	ACharacterV2* Player = Cast<ACharacterV2>(CurrentCharacter);
	FRotator Rotation = Player->GetFollowCamera()->GetComponentRotation();
	TextRender->SetWorldRotation(FRotator(Rotation.Pitch * -1, Rotation.Yaw - 180, 0));
}

