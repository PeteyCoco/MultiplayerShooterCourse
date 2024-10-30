// Copyright Peter Carsten Collins (2024)


#include "Weapon/ProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	// Only run on the server (this is the case since the weapon is replicated and so HasAuthority is only true on the server)
	if (!HasAuthority()) return;

	// Spawn the projectile
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlashSocket"));
	if (MuzzleFlashSocket)
	{
		const FTransform MuzzleFlashSocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector ToTarget = HitTarget - MuzzleFlashSocketTransform.GetLocation();
		const FRotator ToTargetRotation = ToTarget.Rotation();

		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectile>(
					ProjectileClass, 
					MuzzleFlashSocketTransform.GetLocation(),
					ToTargetRotation,
					SpawnParams
				);
			}
		}
	}
}
