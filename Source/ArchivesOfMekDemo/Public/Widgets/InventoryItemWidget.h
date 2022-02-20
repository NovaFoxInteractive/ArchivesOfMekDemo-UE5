// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARCHIVESOFMEKDEMO_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, Category="Inventory|Item", meta=(AllowPrivateAccess="true"), meta=(ExposeOnSpawn=true))
	class UItem* Item;
};
