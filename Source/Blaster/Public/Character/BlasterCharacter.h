// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"

class ABlasterPlayerController;
class ABlasterPlayerState;
class AWeapon;
class UCameraComponent;
class UCombatComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UWidgetComponent;

struct FInputActionValue;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	//~ Begin ACharacter interface
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual void PostInitializeComponents() override;
protected:
	virtual void BeginPlay() override;
	virtual void Jump() override;
	//~ End ACharacter interface

public:
	// Animation montage players
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayHitReactMontage();
	void PlayElimMontage();

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

	// Server elimination logic
	void Elim();

	// Cosmetic elimination logic
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	// Return true if this character has been eliminated
	bool IsEliminated() const { return bIsEliminated; }

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }

	ECombatState GetCombatState() const;

protected:
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

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ReloadAction;
	 
	// Input callback functions
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void EquipButtonPressed(const FInputActionValue& InputActionValue);
	void CrouchButtonPressed(const FInputActionValue& InputActionValue);
	void AimButtonPressed(const FInputActionValue& InputActionValue);
	void AimButtonReleased(const FInputActionValue& InputActionValue);
	void FireButtonPressed(const FInputActionValue& InputActionValue);
	void FireButtonReleased(const FInputActionValue& InputActionValue);
	void ReloadButtonPressed(const FInputActionValue& InputActionValue);
	//~ End Input section

	// Update the aim offset for a frame
	void AimOffset(float DeltaTime);

	// Update the turn in place state for a frame
	void TurnInPlace(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	// Callback to damage event
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	void UpdateHUDHealth();

	// Poll for any relevant classes and initialize HUD
	void PollInit();

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> OverheadWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> Combat;

	// The currently overlapped weapon
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	TObjectPtr<AWeapon> OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	// GameFramework references
	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController;
	UPROPERTY()
	ABlasterPlayerState* BlasterPlayerState;

	// Aim offsets
	float AO_Yaw;
	float AO_Pitch;
	float AO_Yaw_Interp;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlaceState;

	// Animation Montages
	UPROPERTY(EditAnywhere, Category = "Combat | Animation")
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = "Combat | Animation")
	UAnimMontage* ReloadMontage; 

	UPROPERTY(EditAnywhere, Category = "Combat | Animation")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Combat | Animation")
	UAnimMontage* ElimMontage;

	// Hide the character if the camera is too close
	void HideCharacterIfCameraClose();

	// Distance threshold between camera and character for hiding
	UPROPERTY(EditAnywhere, Category = "Combat")
	float CameraThreshold = 200.f;

	/* Begin section: Player Health */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Health, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();
	/* End section: Player Health */

	bool bIsEliminated = false;

	/* Begin section: Elimination timer*/
	FTimerHandle ElimTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Elimination")
	float ElimTimerDelay = 3.f;
	void ElimTimerFinish();
	/* End section: Elimination timer*/

	/* Begin section: Dissolve effect*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> DissolveTimeline;

	UFUNCTION()
	void StartDissolveMaterial();

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);

	FOnTimelineFloat DissolveTrack;

	// Curve determining dissolve effect (time axis is normalized to DissolveRate)
	UPROPERTY(EditAnywhere, Category = "Elimination|Effects")
	TObjectPtr<UCurveFloat> DissolveCurve;

	// The rate of the dissolve effect (1/sec)
	UPROPERTY(EditAnywhere, Category = "Elimination|Effects")
	float DissolveRate = 1.f;

	// Dynamic instance that we can change at runtime
	TArray<UMaterialInstanceDynamic*> DissolveDynamicMaterialInstances;

	void CreateDissolveDynamicMaterialInstances();
	/* End section: Dissolve effect*/

	void DisableMovement();

};
