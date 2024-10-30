// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

/**
 * Base class for weapons that fire a projectile
 */
UCLASS()
class BLASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

//~ Begin AWeapon interface
public:
	virtual void Fire(const FVector& HitTarget) override;
//~ End AWeapon interface

private:
	// The projectile to spawn
	UPROPERTY(EditAnywhere, Category = "Projectile Properties")
	TSubclassOf<AProjectile> ProjectileClass;

};
