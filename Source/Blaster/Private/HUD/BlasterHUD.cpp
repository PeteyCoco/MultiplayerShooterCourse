// Copyright Peter Carsten Collins (2024)


#include "HUD/BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	if (GEngine)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter = ViewportSize / 2.f;

		const float SpreadScaled = HUDPackage.CrosshairSpread * CrosshairSpreadMax;

		DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, FVector2D(0.f, 0.f), HUDPackage.CrosshairsColor);
		DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.f), HUDPackage.CrosshairsColor);
		DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, FVector2D(SpreadScaled, 0.f), HUDPackage.CrosshairsColor);
		DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, FVector2D(0.f, -SpreadScaled), HUDPackage.CrosshairsColor);
		DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, FVector2D(0.f, SpreadScaled), HUDPackage.CrosshairsColor);
	}
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, const FVector2D& DrawLocation, const FVector2D& Spread, const FLinearColor& CrosshairColor)
{
	if (Texture)
	{
		const float TextureWidth = Texture->GetSizeX();
		const float TextureHeight = Texture->GetSizeY();
		const FVector2D TextureDrawLocation
		{
			DrawLocation.X - (TextureWidth / 2.f) + Spread.X,
			DrawLocation.Y - (TextureHeight / 2.f) + Spread.Y
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
			1.f,
			CrosshairColor
		);
	}
}
