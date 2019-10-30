// SillikOne.


#include "GossipZone.h"
#include "CharacterV2.h"
#include "Components/SphereComponent.h"

// Sets default values
AGossipZone::AGossipZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(FName("SphereComponent"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	SphereComponent->SetGenerateOverlapEvents(true);

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AGossipZone::OnOverlapDialogueBegin);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AGossipZone::OnOverlapDialogueEnd);
}

void AGossipZone::OnOverlapDialogueBegin_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	ACharacterV2* OverlapingCharacter = Cast<ACharacterV2>(OtherActor);
	if (!OverlapingCharacter) { return; }

}

void AGossipZone::OnOverlapDialogueEnd_Implementation(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Called when the game starts or when spawned
void AGossipZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGossipZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

