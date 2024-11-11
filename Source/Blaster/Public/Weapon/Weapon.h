// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


class ABlasterCharacter;
class ABlasterPlayerController;
class ACasing;
class USphereComponent;
class UTexture2D;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX"),
};

/*
* Base class for weapons
*/
UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
	//~ Begin AActor interface
public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	virtual void OnRep_Owner() override;
protected:
	virtual void BeginPlay() override;
	//~ End AActor interface

public:
	AWeapon();

	// Show or hide the pickup widget
	void ShowPickupWidget(bool bShowWidget);

	// Fire this weapon
	virtual void Fire(const FVector& HitTarget);

	// Check if this weapon can fire
	bool CanFire() const;

	// Check if this weapon is out of ammo
	bool IsEmpty() const;

	// Logic executed when the weapon is dropped
	void Dropped();

	// Set ammo on the player hud
	void SetHUDAmmo();

	// Begin section: Textures for the weapon's crosshairs
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Crosshairs")
	TObjectPtr<UTexture2D> CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Crosshairs")
	TObjectPtr<UTexture2D> CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Crosshairs")
	TObjectPtr<UTexture2D> CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Crosshairs")
	TObjectPtr<UTexture2D> CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Crosshairs")
	TObjectPtr<UTexture2D> CrosshairsBottom;
	// End section: Textures for the weapon's crosshairs

	// Begin section: Automatic fire
	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireDelay = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	bool bIsAutomatic = true;
	// End section: Automatic fire
	
	//~ Begin overlap callbacks
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//~ End overlap callbacks

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	// Sphere for detecting overlaps with characters
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USphereComponent> AreaSphere;

	// The current state of the weapon
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	// A widget indicating that the weapon can be picked up
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UWidgetComponent> PickupWidget;

	// The firing animation
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<UAnimationAsset> FireAnimation;

	// The actor class for bullet casings
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Casing")
	TSubclassOf<ACasing> CasingClass;

	// The minimum casing ejection impulse strength 
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Casing")
	float MinCasingEjectionImpulse = 8.f;

	// The maximum casing ejection impulse strength 
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Casing")
	float MaxCasingEjectionImpulse = 12.f;

	// The maximum random pitch of the ejected casing
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Casing")
	float CasingPitchMax = 15.f;

	// Lifespan of the casing
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Casing")
	float CasingLifespan = 5.f;

	// Play the firing animation
	void PlayFiringAnimation();

	// Spawn a spent casing at the Ammo Eject socket transform with a random rotation
	void SpawnCasing();

	// Generate a random ejection transform for the casing
	FTransform GenerateRandomEjectionTransform(const FTransform& SocketTransform) const;

	// Add a random impulse to the casing in the direction of the the Ammo Eject socket's right vector
	void AddCasingImpulse(ACasing* Casing, const FTransform& SocketTransform) const;

	/*~ Begin aiming FOV section */
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Aiming")
	float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Aiming")
	float ZoomInterpSpeed = 20.f;
	/*~ End aiming FOV section */

	UPROPERTY(ReplicatedUsing = OnRep_Ammo, EditAnywhere, Category = "Weapon Properties")
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	UFUNCTION()
	void SpendRound();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 AmmoCapacity;

	UPROPERTY()
	ABlasterCharacter* BlasterOwnerCharacter;

	UPROPERTY()
	ABlasterPlayerController* BlasterOwnerController;

public:
	void SetWeaponState(EWeaponState State);

	USphereComponent* GetAreaSphere() const { return AreaSphere; }

	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	float GetZoomedFOV() const { return ZoomedFOV; }

	float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
};
