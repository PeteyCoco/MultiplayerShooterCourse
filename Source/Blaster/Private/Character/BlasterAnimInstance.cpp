// Copyright Peter Carsten Collins (2024)


#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/Weapon.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!BlasterCharacter)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if (!BlasterCharacter) return;

	/*
	*	Collect data from the character
	*/
	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Length();

	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	const float AccelerationMagnitude = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size();
	bIsAccelerating = !FMath::IsNearlyZero(AccelerationMagnitude);

	// Collect weapon data
	bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();

	bIsCrouched = BlasterCharacter->bIsCrouched;
	bIsAiming = BlasterCharacter->IsAiming();

	// Yaw Offset for strafing
	const FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

	// Lean for quick turns
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	// Aim offsets
	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	TurningInPlaceState = BlasterCharacter->GetTurningInPlace();

	// Get the left hand transform data
	if (bIsWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("RightHand"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		const FTransform MuzzleTipSocketTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("MuzzleFlashSocket"), RTS_World);
		const FTransform RightHandTransform = BlasterCharacter->GetMesh()->GetSocketTransform(FName("RightHand"), RTS_World);

		const FVector MuzzleToTarget = BlasterCharacter->GetHitTarget() - MuzzleTipSocketTransform.GetLocation();
		const FVector MuzzleDirection = MuzzleTipSocketTransform.GetRotation().GetForwardVector();
		const FVector RightHandX = RightHandTransform.GetRotation().GetAxisX();

		const FQuat AimDeltaRotatorMS = FQuat::FindBetween(MuzzleDirection, MuzzleToTarget); 
		const FQuat AimDeltaRotatorWS = AimDeltaRotatorMS * MuzzleTipSocketTransform.GetRotation(); // Rotates world space to align with aim target
		const FQuat RightHandRotatorWS = RightHandTransform.Inverse().GetRotation(); // Rotates vector in hand space to world space

		FRotator RightHandRotation = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, BlasterCharacter->GetHitTarget());

		const FVector MuzzleX(MuzzleTipSocketTransform.GetRotation().GetAxisX());
		DrawDebugLine(GetWorld(), MuzzleTipSocketTransform.GetLocation(), MuzzleTipSocketTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
		DrawDebugLine(GetWorld(), MuzzleTipSocketTransform.GetLocation(), BlasterCharacter->GetHitTarget(), FColor::Orange);
		DrawDebugLine(GetWorld(), RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + FVector::XAxisVector * 1000.f, FColor::Blue); // World X-axis
		DrawDebugLine(GetWorld(), RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + AimDeltaRotatorWS * FVector::XAxisVector * 1000.f, FColor::Yellow); // Should be parallel to the muzzle's forward direction
		DrawDebugLine(GetWorld(), RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + RightHandRotatorWS * RightHandX * 1000.f, FColor::Green); // Should match world X-axis


	}
}
