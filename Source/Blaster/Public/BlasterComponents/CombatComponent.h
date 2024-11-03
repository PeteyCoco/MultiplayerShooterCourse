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

	/* Value for the baseline crosshair spread.*/
	float CrosshairBaselineSpread = 0.5;

	/* Value dictating how spread the crosshair should be due to movement velocity*/
	float CrosshairVelocityFactor;
	float MaxCrosshairVelocityFactor = 1.f;
	// Tick updater for crosshair velocity factor
	void UpdateCrosshairVelocityFactor(float DeltaTime);

	/* Value dictating how spread the crosshair should be due to movement velocity*/
	float CrosshairInAirFactor;
	float MaxCrosshairInAirFactor = 2.f;
	// Tick updater for crosshair in air factor
	void UpdateCrosshairInAirFactor(float DeltaTime);

	/* Value dictating how spread the crosshair should be due to aiming */
	float CrosshairAimFactor;
	float MaxCrosshairAimFactor = -0.58f;
	// Tick updater for crosshair in air factor
	void UpdateCrosshairAimFactor(float DeltaTime);

	/* Value dictating how spread the crosshair should be due to firing */
	float CrosshairShootFactor;
	float MaxCrosshairShootFactor = 1.f;
	// Tick updater for crosshair in air factor
	void UpdateCrosshairShootFactor(float DeltaTime);

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

	// The aim target in world space
	FVector HitTarget;

	/* Begin Aiming and FOV section */

	// Field of view when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;

	// The current FOV
	float CurrentFOV;

	// The FOV in the aiming state
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;

	// The interpolation speed for camera zoom
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedInterpSpeed = 20.f;

	// Update the FOV each frame
	void UpdateCameraFOV(float DeltaTime);

	/* End Aiming and FOV section */
};
