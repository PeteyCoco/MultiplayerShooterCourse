// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"


class USoundCue;

UCLASS()
class BLASTER_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACasing();

//~ Begin AActor interface
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
//~ End AActor interface

protected:
	// Overlap callbacks
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere, Category = "Casing Properties")
	TObjectPtr<UStaticMeshComponent> CasingMesh;

	UPROPERTY(EditAnywhere, Category = "Casing Properties")
	float CasingEjectionImpulse;

	// Sound played on shell impact
	UPROPERTY(EditAnywhere, Category = "Casing Properties")
	TObjectPtr<USoundCue> CasingSound;

	// Flag to ensure casing sound is only played once
	bool bCasingSoundPlayed = false;
};
