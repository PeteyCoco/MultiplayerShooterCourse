// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(Display = "Turning Left"),
	ETIP_Right UMETA(Display = "Turning Right"),
	ETIP_NotTurning UMETA(Display = "Not Turning"),

	ETIP_MAX UMETA(Display = "DefaultMAX")
};
