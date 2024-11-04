// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

class ABlasterHUD;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

	//~ Begin APlayerController interface
protected:
	virtual void BeginPlay() override;
	//~ End APlayerController interface

public:

	void SetHUDHealth(float Health, float MaxHealth);

private:
	ABlasterHUD* BlasterHUD;
};
