// Copyright NovaFox Interactive L.L.C 2021
#pragma once

#include "World/Pickup.h"
#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Epic UMETA(DisplayName = "Epic"),
	EIR_Legendary UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemRarity Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText UseActionText;
	
	// Only used if dropping item results in spawning a pickup
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APickup> PickupMesh; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxStackAmount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeUsed = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeDropped = true;
};