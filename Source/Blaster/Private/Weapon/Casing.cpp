// Copyright Peter Carsten Collins (2024)


#include "Weapon/Casing.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(RootComp);

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>("CasingMesh");
	CasingMesh->SetupAttachment(GetRootComponent());

	CasingMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
}

UStaticMeshComponent* ACasing::GetMesh()
{
	return CasingMesh;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (CasingSound && !bCasingSoundPlayed)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CasingSound, GetActorLocation());
		bCasingSoundPlayed = true;
	}
}

