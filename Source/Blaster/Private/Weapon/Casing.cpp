// Copyright Peter Carsten Collins (2024)


#include "Weapon/Casing.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>("CasingMesh");
	SetRootComponent(CasingMesh);

	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);

	CasingEjectionImpulse = 10.f;

	SetLifeSpan(5.f);
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);

	CasingMesh->AddImpulse(CasingEjectionImpulse * GetActorForwardVector());
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (CasingSound && !bCasingSoundPlayed)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CasingSound, GetActorLocation());
		bCasingSoundPlayed = true;
	}
}

