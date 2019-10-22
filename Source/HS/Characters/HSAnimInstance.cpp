// SillikOne.


#include "HSAnimInstance.h"

void UHSAnimInstance::NativeInitializeAnimation()
{
	//Very Important Line
	Super::NativeInitializeAnimation();

	//Cache the owning pawn for use in Tick
	OwningPawn = TryGetPawnOwner();
}

////////////////////////////////
// Tick
void UHSAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	//Very Important Line
	Super::NativeUpdateAnimation(DeltaTimeX);

	//Always Check Pointers
	if (!OwningPawn) { return;}
}

void UHSAnimInstance::Notify_SwitchWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("NOTIFY SWITCHWEAPON!"))
}
