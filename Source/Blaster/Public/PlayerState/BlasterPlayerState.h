// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()

	//~ Begin APlayerState interface
public:
	virtual void OnRep_Score() override;
	//~ End APlayerState interface

public:
	void AddToScore(float Amount);

private:
	ABlasterCharacter* Character;
	ABlasterPlayerController* Controller;
	
};
