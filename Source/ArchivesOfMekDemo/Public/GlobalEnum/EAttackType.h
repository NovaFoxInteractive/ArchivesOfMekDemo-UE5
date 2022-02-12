// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EAttackType.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8 {
	EAT_None UMETA(DisplayName = "None"),
	EAT_LightAttack UMETA(DisplayName = "LightAttack"),
	EAT_HeavyAttack UMETA(DisplayName = "HeavyAttack"),
	EAT_LightJumpAttack UMETA(DisplayName = "LightJumpAttack"),
	EAT_HeavyJumpAttack UMETA(DisplayName = "HeavyJumpAttack")
};
