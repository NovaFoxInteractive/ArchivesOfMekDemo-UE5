// Copyright NovaFox Interactive L.L.C 2021
#pragma once

#include "Item/Item.h"
#include "InventoryItems.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItems
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AItem> Item;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount = 0;

	FInventoryItems() = default;
	FInventoryItems(TSubclassOf<AItem> Item, int32 Amount) : Item(Item), Amount(Amount) {}
};