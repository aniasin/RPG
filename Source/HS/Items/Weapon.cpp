// SillikOne.


#include "Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "CharacterV2.h"
#include "Classes/Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComponent;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	CapsuleComponent->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
	ACharacterV2* Player = Cast<ACharacterV2>(OtherActor);
	if (HasAuthority())
	{
		if (!Player) { return; }
		CurrentOverlapingActors.Add(Player);

		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("OverlapBegin with %s!"), *(OtherActor->GetName())));
		UE_LOG(LogTemp, Warning, TEXT("Overlap Begin with %s!"), *OtherActor->GetName())
		Player->ToggleInteractionWidget(this);
	}
}

void AWeapon::OnOverlapEnd_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	ACharacterV2* Player = Cast<ACharacterV2>(OtherActor);
	if (HasAuthority())
	{
		if (!Player) { return; }

		CurrentOverlapingActors.Remove(Player);
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("OverlapEnd with %s!"), *(OtherActor->GetName())));
		UE_LOG(LogTemp, Warning, TEXT("Overlap End with %s!"), *OtherActor->GetName())
		Player->ToggleInteractionWidget(this);
	}
}

void AWeapon::ItemTaken()
{
	AHSCharacterBase* CurrentOwner = Cast<AHSCharacterBase>(OwnerActor);
	CurrentOwner->GrantAbilities(Abilities);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::ItemDropped()
{
	UE_LOG(LogTemp, Warning, TEXT("Server Calling Destroy!"))
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	ConditionalBeginDestroy();
}
