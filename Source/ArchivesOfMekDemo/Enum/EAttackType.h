// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EAttackType.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8 {
	EA_None UMETA(DisplayName = "None"),
	EA_LightAttack UMETA(DisplayName = "LightAttack"),
	EA_HeavyAttack UMETA(DisplayName = "HeavyAttack"),
	EA_LightJumpAttack UMETA(DisplayName = "LightJumpAttack"),
	EA_HeavyJumpAttack UMETA(DisplayName = "HeavyJumpAttack")
};
