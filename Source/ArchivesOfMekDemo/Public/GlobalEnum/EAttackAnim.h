// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EAttackAnim.generated.h"

UENUM(BlueprintType)
enum class EAttackAnim : uint8 {
	EAA_L UMETA(DisplayName = "Light"),
	EAA_LL UMETA(DisplayName = "LightLight"),
	EAA_LLL UMETA(DisplayName = "LightLightLight"),
	EAA_LLH UMETA(DisplayName = "LightLightHeavy"),
	EAA_LJump UMETA(DisplayName = "LightJump"),

	EAA_H UMETA(DisplayName = "Heavy"),
	EAA_HH UMETA(DisplayName = "HeavyHeavy"),
	EAA_HHH UMETA(DisplayName = "HeavyHeavyHeavy"),
	EAA_HHL UMETA(DisplayName = "HeavyHeavyLight"),
	EAA_HJump UMETA(DisplayName = "HeavyJump")
};