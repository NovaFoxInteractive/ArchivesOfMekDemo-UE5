// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EDifficulty.generated.h"

UENUM(BlueprintType)
enum class EDifficulty : uint8 {
	ED_Normal UMETA(DisplayName = "Normal"),
	ED_Pro UMETA(DisplayName = "Pro")
};