// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


class ACasing;

class USphereComponent;
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
	
public:	
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// Show or hide the pickup widget
	void ShowPickupWidget(bool bShowWidget);

	// Fire this weapon
	virtual void Fire(const FVector& HitTarget);

protected:
	virtual void BeginPlay() override;
	
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

public:
	void SetWeaponState(EWeaponState State);

	USphereComponent* GetAreaSphere() const { return AreaSphere; }

	USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
};
