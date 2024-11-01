// Copyright Peter Carsten Collins (2024)


#include "HUD/BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter = ViewportSize / 2.f;

		DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter);
		DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter);
		DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter);
		DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter);
		DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter);
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, const FVector2D& DrawLocation)
{
	if (Texture)
	{
		const float TextureWidth = Texture->GetSizeX();
		const float TextureHeight = Texture->GetSizeY();
		const FVector2D TextureDrawLocation
		{
			DrawLocation.X - (TextureWidth / 2.f),
			DrawLocation.Y - (TextureHeight / 2.f)
		};
		DrawTexture(
			Texture,
			TextureDrawLocation.X,
			TextureDrawLocation.Y,
			TextureWidth,
			TextureHeight,
			0.f,
			0.f,
			1.f,
			1.f
		);
	}
}
