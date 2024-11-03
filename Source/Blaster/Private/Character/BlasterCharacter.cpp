// Copyright Peter Carsten Collins (2024)


#include "Character/BlasterCharacter.h"

#include "BlasterComponents/CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/BlasterAnimInstance.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

ABlasterCharacter::ABlasterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true; // Rotate the boom with the controller rotation

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	Combat->SetIsReplicated(true);

	// Character Movement properties
	bUseControllerRotationYaw = false; // Character does not rotate with camera
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 850.f, 0.f);
	TurningInPlaceState = ETurningInPlace::ETIP_NotTurning;

	// Collision properties
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	// Net properties
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (Combat)
	{
		Combat->Character = this;
	}
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Register replicated variables
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add the input mapping context
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
	
}

void ABlasterCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
	TurnInPlace(DeltaTime);
}

void ABlasterCharacter::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	if (!InputAxisVector.IsNearlyZero())
	{
		// Use the control rotation to get the forward and right direction vectors
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Perform movement
		AddMovementInput(ForwardDirection, InputAxisVector.Y);
		AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void ABlasterCharacter::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	if (!InputAxisVector.IsNearlyZero())
	{
		GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::White, FString::Printf(TEXT("X: %f, Y: %f"), InputAxisVector.X, InputAxisVector.Y));
		// Perform rotation
		AddControllerYawInput(InputAxisVector.X);
		AddControllerPitchInput(InputAxisVector.Y);
	}
}

void ABlasterCharacter::EquipButtonPressed(const FInputActionValue& InputActionValue)
{
	if (Combat)
	{
		if (HasAuthority())
		{
			// Server
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			// Client
			ServerEquipButtonPressed();
		}
	}
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat) Combat->EquipWeapon(OverlappingWeapon);
}

void ABlasterCharacter::CrouchButtonPressed(const FInputActionValue& InputActionValue)
{
	bIsCrouched ? UnCrouch() : Crouch();
}

void ABlasterCharacter::AimButtonPressed(const FInputActionValue& InputActionValue)
{
	if (Combat) Combat->SetAiming(true);
}

void ABlasterCharacter::AimButtonReleased(const FInputActionValue& InputActionValue)
{
	if (Combat) Combat->SetAiming(false);
}

void ABlasterCharacter::FireButtonPressed(const FInputActionValue& InputActionValue)
{
	if (Combat) Combat->FireButtonPressed(true);
}

void ABlasterCharacter::FireButtonReleased(const FInputActionValue& InputActionValue)
{
	if (Combat) Combat->FireButtonPressed(false);
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
	if (Combat->EquippedWeapon == nullptr) return;

	// Calculate properties
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	const float Speed = Velocity.Length();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (FMath::IsNearlyZero(Speed) && !bIsInAir) // Standing still and not jumping
	{
		bUseControllerRotationYaw = true;

		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlaceState == ETurningInPlace::ETIP_NotTurning)
		{
			AO_Yaw_Interp = AO_Yaw;
		}
	}
	else // running or jumping
	{
		bUseControllerRotationYaw = true;

		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
	}

	// Pitch is always updated
	float AO_PitchTarget = GetBaseAimRotation().Pitch;
	if (!IsLocallyControlled() && AO_PitchTarget > 90.f)
	{
		// Remap replicated pitch value to the original range
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		AO_PitchTarget = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_PitchTarget);
	}
	AO_Pitch = FMath::FInterpTo(AO_Pitch, AO_PitchTarget, DeltaTime, 10.f);
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlaceState = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlaceState = ETurningInPlace::ETIP_Left;
	}
	else
	{
		TurningInPlaceState = ETurningInPlace::ETIP_NotTurning;
	}

	if (TurningInPlaceState != ETurningInPlace::ETIP_NotTurning)
	{
		AO_Yaw_Interp = FMath::FInterpTo(AO_Yaw_Interp, 0.f, DeltaTime, 4.f);
		AO_Yaw = AO_Yaw_Interp;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlaceState = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABlasterCharacter::PlayFireMontage(bool bAiming)
{
	if (!Combat || !Combat->EquippedWeapon) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsLocallyControlled()) // Only true on the server for the server's character
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(false);
		}
	}
	OverlappingWeapon = Weapon;

	if (IsLocallyControlled()) 
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ABlasterCharacter::IsWeaponEquipped() const
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABlasterCharacter::IsAiming() const
{
	return (Combat && Combat->bIsAiming);
}

TObjectPtr<AWeapon> ABlasterCharacter::GetEquippedWeapon() const
{
	return Combat ? Combat->EquippedWeapon : nullptr;
}

FVector ABlasterCharacter::GetHitTarget() const
{
	return Combat ? Combat->HitTarget : FVector();
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	// Remember that the server never receives a rep notify
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	check(EnhancedInputComponent)

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABlasterCharacter::Jump);

	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Completed, this, &ABlasterCharacter::EquipButtonPressed);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABlasterCharacter::CrouchButtonPressed);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ABlasterCharacter::AimButtonPressed);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABlasterCharacter::AimButtonReleased);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ABlasterCharacter::FireButtonPressed);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABlasterCharacter::FireButtonReleased);
}

