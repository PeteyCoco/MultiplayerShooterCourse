// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};
