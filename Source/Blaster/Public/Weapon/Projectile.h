// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class UProjectileMovementComponent;

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

//~ Begin AActor interface
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
//~ End AActor interface

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> Tracer;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraComponent> TracerComponent;
};
