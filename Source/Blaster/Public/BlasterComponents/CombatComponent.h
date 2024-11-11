// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "HUD/BlasterHUD.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABlasterCharacter;
class ABlasterHUD;
class ABlasterPlayerController;
class AWeapon;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UActorComponent interface

	/** Equips a weapon to the character */
	void EquipWeapon(AWeapon* WeaponToEquip);

	// Fire functions
	void FireButtonPressed(bool bInIsFireButtonPressed);
	void Fire();

	bool CanFire() const;

protected:
	// Aiming state functions
	void SetAiming(bool bInIsAiming);

	// RPCs
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bInIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	// Fire timer management
	void FireTimerStart();
	void FireTimerFinish();

	// HUD and crosshair functions
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void UpdateHUDCrosshairs(float DeltaTime);
	void UpdateCrosshairVelocityFactor(float DeltaTime);
	void UpdateCrosshairInAirFactor(float DeltaTime);
	void UpdateCrosshairAimFactor(float DeltaTime);
	void UpdateCrosshairShootFactor(float DeltaTime);

	// Camera and FOV
	void UpdateCameraFOV(float DeltaTime);

private:
	// References
	ABlasterCharacter* Character = nullptr;
	ABlasterPlayerController* Controller = nullptr;
	ABlasterHUD* HUD = nullptr;

	// Weapon and aiming states
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon = nullptr;
	UPROPERTY(Replicated)
	bool bIsAiming = false;

	// Fire button state and timer
	bool bIsFireButtonPressed = false;
	FTimerHandle FireTimer;
	bool bIsFiring = false;

	// HUD crosshair properties
	FHUDPackage HUDPackage;
	float CrosshairVelocityFactor = 0.f;
	float CrosshairInAirFactor = 0.f;
	float CrosshairAimFactor = 0.f;
	float CrosshairShootFactor = 0.f;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed = 600.f;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed = 400.f;

	// Crosshair spread properties
	float CrosshairBaselineSpread = 0.5f;
	float MaxCrosshairVelocityFactor = 1.f;
	float MaxCrosshairInAirFactor = 2.f;
	float MaxCrosshairAimFactor = -0.58f;
	float MaxCrosshairShootFactor = 1.f;

	// Aiming and FOV properties
	float DefaultFOV;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ZoomedInterpSpeed = 20.f;

	// Targeting
	FVector HitTarget;
};
