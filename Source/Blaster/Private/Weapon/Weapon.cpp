// Copyright Peter Carsten Collins (2024)


#include "Weapon/Weapon.h"

#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Casing.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Start off without collision, but have the ECRs setup

	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore); 
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Start off with a disabled AreaSphere

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(RootComponent);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}

	if (HasAuthority())
	{
		// Collisions are only enabled on the server
		AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereBeginOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget) PickupWidget->SetVisibility(bShowWidget);
}

void AWeapon::Fire(const FVector& HitTarget)
{
	PlayFiringAnimation();
	SpawnCasing();
}

void AWeapon::PlayFiringAnimation()
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
}

void AWeapon::SpawnCasing()
{
	const USkeletalMeshSocket* AmmoEjectSocket = GetWeaponMesh()->GetSocketByName(FName("AmmoEjectSocket"));
	if (!AmmoEjectSocket || !CasingClass) return;

	// Set Casing spawn transform with random rotation
	const FTransform AmmoEjectSocketTransform = AmmoEjectSocket->GetSocketTransform(GetWeaponMesh());
	FTransform CasingTransform = GenerateRandomEjectionTransform(AmmoEjectSocketTransform);

	UWorld* World = GetWorld();
	if (World)
	{
		ACasing* Casing = World->SpawnActor<ACasing>(
			CasingClass,
			CasingTransform.GetLocation(),
			CasingTransform.GetRotation().Rotator()
		);
		Casing->SetLifeSpan(CasingLifespan);
		if (Casing)
		{
			AddCasingImpulse(Casing, AmmoEjectSocketTransform);
		}
	}
}

FTransform AWeapon::GenerateRandomEjectionTransform(const FTransform& SocketTransform) const
{
	const float RandomPitch = FMath::FRandRange(0.f, CasingPitchMax);
	const float RandomRoll = FMath::FRandRange(0.f, 360.f);
	const FRotator RandomRotator(RandomPitch, 0.f, RandomRoll);

	return FTransform(SocketTransform.GetRotation().Rotator() + RandomRotator, SocketTransform.GetLocation());
}

void AWeapon::AddCasingImpulse(ACasing* Casing, const FTransform& SocketTransform) const
{
	const float RandomEjectionImpulse = FMath::FRandRange(MinCasingEjectionImpulse, MaxCasingEjectionImpulse);
	const FVector EjectionDirection = SocketTransform.GetRotation().GetRightVector();
	Casing->GetMesh()->AddImpulse(RandomEjectionImpulse * EjectionDirection);
}

