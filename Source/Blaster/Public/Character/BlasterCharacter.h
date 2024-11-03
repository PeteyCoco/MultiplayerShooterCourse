// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCharacter.generated.h"

class AWeapon;

class UCameraComponent;
class UCombatComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UWidgetComponent;

struct FInputActionValue;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	//~ Begin ACharacter interface
	virtual void Jump() override;
	//~ End ACharacter interface

	//~ Begin Input section
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction;
	 
	// Input callback functions
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void EquipButtonPressed(const FInputActionValue& InputActionValue);
	void CrouchButtonPressed(const FInputActionValue& InputActionValue);
	void AimButtonPressed(const FInputActionValue& InputActionValue);
	void AimButtonReleased(const FInputActionValue& InputActionValue);
	void FireButtonPressed(const FInputActionValue& InputActionValue);
	void FireButtonReleased(const FInputActionValue& InputActionValue);
	//~ End Input section

	// Update the aim offset for a frame
	void AimOffset(float DeltaTime);

	// Update the turn in place state for a frame
	void TurnInPlace(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<UCombatComponent> Combat;

	// The currently overlapped weapon
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	// Aim offsets
	float AO_Yaw;
	float AO_Pitch;
	float AO_Yaw_Interp;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlaceState;

	UPROPERTY(EditAnywhere, Category = "Combat")
	class UAnimMontage* FireWeaponMontage;

public:
	// Play the fire weapon montage
	void PlayFireMontage(bool bAiming);

	// Set the overlapping weapon
	void SetOverlappingWeapon(AWeapon* Weapon);

	// Return true if a weapon is equipped
	bool IsWeaponEquipped() const;

	// Return true if the character is aiming
	bool IsAiming() const;

	// Return the yaw aim offset
	float GetAO_Yaw() const { return AO_Yaw; }

	// Return the pitch aim offset
	float GetAO_Pitch() const { return AO_Pitch; }

	// Return the turning in place state
	ETurningInPlace GetTurningInPlace() const { return TurningInPlaceState; }

	// Return the equipped weapon
	TObjectPtr<AWeapon> GetEquippedWeapon() const;

	// Return the world location of the aim target
	FVector GetHitTarget() const;

	// Get the follow camera
	UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
