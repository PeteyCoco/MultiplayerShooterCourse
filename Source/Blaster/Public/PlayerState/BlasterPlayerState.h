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
	virtual void GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End APlayerState interface

public:
	// Server-side statistics setters
	void AddToScore(float Amount);
	void AddToDeaths(int32 Amount);

private:
	// GameFramework references
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	ABlasterPlayerController* Controller;

	// Deaths statistic
	UPROPERTY(ReplicatedUsing = OnRep_Deaths)
	int32 Deaths = 0;
	UFUNCTION()
	void OnRep_Deaths();
	
};
