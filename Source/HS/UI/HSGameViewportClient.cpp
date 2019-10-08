// SillikOne.

#include "HSGameViewportClient.h"
#include "HS.h"
#include "Engine/World.h"
#include "Engine/Canvas.h"


void UHSGameViewportClient::PostRender(UCanvas* Canvas)
{
	Super::PostRender(Canvas);

	// Fade if requested, you could use the same DrawScreenFade method from any canvas such as the HUD
	if (bFading)
	{
		DrawScreenFade(Canvas);
	}
}

void UHSGameViewportClient::ClearFade()
{
	bFading = false;
}

void UHSGameViewportClient::Fade(const float Duration, const bool bToBlack)
{
	const UWorld* ThisWorld = GetWorld();
	if (ThisWorld)
	{
		bFading = true;
		this->bThisToBlack = bToBlack;
		FadeDuration = Duration;
		FadeStartTime = ThisWorld->GetTimeSeconds();
	}
}

void UHSGameViewportClient::DrawScreenFade(UCanvas* Canvas)
{
	if (bFading)
	{
		const UWorld* ThisWorld = GetWorld();
		if (ThisWorld)
		{
			const float Time = ThisWorld->GetTimeSeconds();
			const float Alpha = FMath::Clamp((Time - FadeStartTime) / FadeDuration, 0.f, 1.f);

			// Make sure that we stay black in a fade to black
			if (Alpha == 1.f && !bThisToBlack)
			{
				bFading = false;
			}
			else
			{
				FColor OldColor = Canvas->DrawColor;
				FLinearColor FadeColor = FLinearColor::Black;
				FadeColor.A = bThisToBlack ? Alpha : 1 - Alpha;
				Canvas->DrawColor = FadeColor.ToFColor(true); // TheJamsh: "4.10 cannot convert directly to FColor, so need to use FLinearColor::ToFColor() :)
				Canvas->DrawTile(Canvas->DefaultTexture, 0, 0, Canvas->ClipX, Canvas->ClipY, 0, 0, Canvas->DefaultTexture->GetSizeX(), Canvas->DefaultTexture->GetSizeY());
				Canvas->DrawColor = OldColor;
			}
		}
	}
}
