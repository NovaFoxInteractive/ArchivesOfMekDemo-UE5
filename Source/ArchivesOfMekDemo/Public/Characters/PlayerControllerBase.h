// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerControllerBase();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta =(AllowPrivateAccess = "true"))
	int32 InventorySlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory, meta =(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> InventoryWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta =(AllowPrivateAccess = "true"))
	UUserWidget* InventoryWidgetRef;
	
	bool bIsInventoryOpen;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory, meta =(AllowPrivateAccess = "true"))
	bool bCanOpenInventory;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void ToggleInventory();

	UFUNCTION(BlueprintImplementableEvent)
	void HideItemInfo();

public:
	FORCEINLINE int32 GetInventorySlots() const { return InventorySlots; }
	FORCEINLINE UUserWidget* GetInventoryWidgetRef() const { return InventoryWidgetRef; }

	FORCEINLINE void SetInventorySlots(const int32 Value) { InventorySlots = Value;}
	FORCEINLINE void SetIsInventoryOpen(const bool Val) { bIsInventoryOpen = Val; }
	FORCEINLINE void SetCanOpenInventory(const bool Val) { bCanOpenInventory = Val; }
};
