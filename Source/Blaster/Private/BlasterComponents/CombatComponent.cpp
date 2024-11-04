// Copyright Peter Carsten Collins (2024)


#include "BlasterComponents/CombatComponent.h"

#include "Camera/CameraComponent.h"
#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/BlasterPlayerController.h"
#include "TimerManager.h"
#include "Weapon/Weapon.h"

#define TRACE_LENGTH 80000.f;

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming)
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		UpdateCameraFOV(DeltaTime);
		UpdateHUDCrosshairs(DeltaTime);
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (!Character || !WeaponToEquip) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bInIsFireButtonPressed)
{
	bIsFireButtonPressed = bInIsFireButtonPressed;
	if (bIsFireButtonPressed && EquippedWeapon)
	{
		Fire();
	}
}

void UCombatComponent::Fire()
{
	if (bCanFire)
	{
		bCanFire = false;
		ServerFire(HitTarget);
		if (EquippedWeapon)
		{
			CrosshairShootFactor = MaxCrosshairShootFactor;
		}
		FireTimerStart();
	}
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character)
	{
		Character->PlayFireMontage(bIsAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UCombatComponent::FireTimerStart()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinish, EquippedWeapon->FireDelay);
}

void UCombatComponent::FireTimerFinish()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	if (bIsFireButtonPressed && EquippedWeapon->bIsAutomatic)
	{
		Fire();
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	// Get the crosshair in world space
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	const FVector2D CrosshairLocation(ViewportSize.X/2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	// Do the line trace
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		if (Character)
		{
			const float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}

		const FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECC_Visibility);

		if (IInteractWithCrosshairsInterface* InteractCrosshairsInterface = Cast<IInteractWithCrosshairsInterface>(TraceHitResult.GetActor()))
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}

		// Set the endpoint if no hit occured
		if (!TraceHitResult.bBlockingHit) TraceHitResult.ImpactPoint = End;
	}
}

void UCombatComponent::SetAiming(bool bInIsAiming)
{
	// First make changes on client so that client sees immediate response to action
	bIsAiming = bInIsAiming;
	if (Character && EquippedWeapon)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}

	// Then send an RPC to server
	ServerSetAiming(bIsAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
	if (Character && EquippedWeapon)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::UpdateHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;

	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}
			UpdateCrosshairVelocityFactor(DeltaTime);
			UpdateCrosshairInAirFactor(DeltaTime);
			UpdateCrosshairAimFactor(DeltaTime);
			UpdateCrosshairShootFactor(DeltaTime);

			HUDPackage.CrosshairSpread = CrosshairBaselineSpread +
				CrosshairVelocityFactor +
				CrosshairInAirFactor +
				CrosshairAimFactor +
				CrosshairShootFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::UpdateCrosshairVelocityFactor(float DeltaTime)
{
	const FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
	const FVector2D VelocityMultiplierRange(0.f, MaxCrosshairVelocityFactor);
	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
}

void UCombatComponent::UpdateCrosshairInAirFactor(float DeltaTime)
{
	float TargetFactor = Character->GetCharacterMovement()->IsFalling() ? MaxCrosshairInAirFactor : 0.f;
	float InterpSpeed = Character->GetCharacterMovement()->IsFalling() ? 2.25f : 30.f;
	CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, TargetFactor, DeltaTime, InterpSpeed);
}

void UCombatComponent::UpdateCrosshairAimFactor(float DeltaTime)
{
	float TargetFactor = bIsAiming ? MaxCrosshairAimFactor : 0.f;
	CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, TargetFactor, DeltaTime, 30.f);
}

void UCombatComponent::UpdateCrosshairShootFactor(float DeltaTime)
{
	CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.f, DeltaTime, 30.f);
}

void UCombatComponent::UpdateCameraFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomedInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}