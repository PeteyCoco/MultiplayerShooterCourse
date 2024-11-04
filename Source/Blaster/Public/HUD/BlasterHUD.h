// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"


class UCharacterOverlay;
class UTexture2D;

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	// Crosshair textures
	TObjectPtr<UTexture2D> CrosshairsCenter;
	TObjectPtr<UTexture2D> CrosshairsLeft;
	TObjectPtr<UTexture2D> CrosshairsRight;	
	TObjectPtr<UTexture2D> CrosshairsTop;
	TObjectPtr<UTexture2D> CrosshairsBottom;

	// Amount to spread the crosshairs
	float CrosshairSpread;

	// Crosshairs color
	FLinearColor CrosshairsColor;
};

/**
 * HUD class for the Blaster character
 */
UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

	//~ Begin AHUD interface
public:
	virtual void DrawHUD() override;
protected:
	virtual void BeginPlay() override;
	//~ End AHUD interface

public:
	void SetHUDPackage(const FHUDPackage& InHUDPackage) { HUDPackage = InHUDPackage; }

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UCharacterOverlay* CharacterOverlay;

protected:

	void AddCharacterOverlay();

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread, const FLinearColor& CrosshairColor);

	UPROPERTY(EditAnywhere, Category = "Crosshair")
	float CrosshairSpreadMax = 16.f;
};
