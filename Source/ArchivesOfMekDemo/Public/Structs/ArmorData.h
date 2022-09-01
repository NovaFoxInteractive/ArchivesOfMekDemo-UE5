// Copyright NovaFox Interactive L.L.C 2021
#pragma once

#include "ArmorData.generated.h"

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	EAT_Helmet UMETA(DisplayName="Helmet"),
	EAT_Chestplate UMETA(DisplayName="Chestplate"),
	EAT_Gauntlet UMETA(DisplayName="Gauntlet"),
	EAT_Legs UMETA(DisplayName="Legs"),
	EAT_Feet UMETA(DisplayName="Feet")
};

USTRUCT(BlueprintType)
struct FArmorData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArmorType ArmorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ArmorValue;
};