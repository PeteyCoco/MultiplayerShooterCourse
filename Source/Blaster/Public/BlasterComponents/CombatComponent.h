// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABlasterCharacter;
class ABlasterHUD;
class ABlasterPlayerController;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	friend class ABlasterCharacter;

	//~ Begin UActorComponent interface
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	//~ End UActorComponent interface

public:
	void EquipWeapon(AWeapon* WeaponToEquip);

protected:
	// Aiming state functions
	void SetAiming(bool bInIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bInIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// Fire button pressed functions
	void FireButtonPressed(bool bPressed);
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	/* Begin HUD and Crosshairs section */

	// Get a hit result under the crosshairs
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	// Update the crosshairs on the HUD each frame
	void UpdateHUDCrosshairs(float DeltaTime);

	// Update the CrosshairVelocityFactor 

	/* Value in range(0, MaxCrosshairVelocityFactor) dictating how spread the crosshair should be due to movement velocity
	(0=No spread, MaxCrosshairVelocityFactor=Maximal spread) */
	float CrosshairVelocityFactor;
	float MaxCrosshairVelocityFactor = 1.f;
	// Tick updater for crosshair velocity factor
	void UpdateCrosshairVelocityFactor(float DeltaTime);

	/* Value in range(0, MaxCrosshairInAirFactor) dictating how spread the crosshair should be due to movement velocity
	(0=No spread, MaxCrosshairInAirFactor=Maximal spread) */
	float CrosshairInAirFactor;
	float MaxCrosshairInAirFactor = 2.f;
	// Tick updater for crosshair in air factor
	void UpdateCrosshairInAirFactor(float DeltaTime);

	/* End HUD and Crosshairs section */

private:
	// Reference to the owning character
	ABlasterCharacter* Character;

	// Reference to the owning player controller
	ABlasterPlayerController* Controller;

	// Reference to the HUD of the owning player controller
	ABlasterHUD* HUD;

	// The currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	// True if the character is aiming
	UPROPERTY(Replicated)
	bool bIsAiming;

	// True if the fire button is pressed
	bool bIsFireButtonPressed;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
};
