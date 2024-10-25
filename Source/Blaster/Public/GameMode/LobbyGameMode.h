// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * GameMode that counts connected players
 */
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	//~ Begin AGameMode interface
	virtual void PostLogin(APlayerController* NewPlayer) override;
	//~ End AGameMode interface
};
