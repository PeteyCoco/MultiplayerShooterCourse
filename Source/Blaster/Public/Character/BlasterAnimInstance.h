// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

/**
 * AnimInstance for the Blaster character
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	//~ Begin UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	//~ End UAnimInstance interface

private:
	// The character using this anim instance
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = true))
	class ABlasterCharacter* BlasterCharacter;

	// The movement speed
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties", meta = (AllowPrivateAccess = true))
	float Speed;

	// True if the character is in the air
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties", meta = (AllowPrivateAccess = true))
	bool bIsInAir;

	// True if the character is accelerating
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties", meta = (AllowPrivateAccess = true))
	bool bIsAccelerating;

	// True if the character has a weapon equipped
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties", meta = (AllowPrivateAccess = true))
	bool bIsWeaponEquipped;

	// True if the character is crouched
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties", meta = (AllowPrivateAccess = true))
	bool bIsCrouched;

	// True if the character is crouched
	UPROPERTY(BlueprintReadOnly, Category = "Character Properties", meta = (AllowPrivateAccess = true))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, Category = "Character Properties", meta = (AllowPrivateAccess = true))
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Character Properties", meta = (AllowPrivateAccess = true))
	float Lean;

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
};
