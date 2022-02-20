// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemsAdded UMETA(DisplayName = "No items added"),
	IAR_SomeItemsAdded UMETA(DisplayName = "Some items added"),
	IAR_AllItemsAdded UMETA(DisplayName = "All items added")
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

public:
	FItemAddResult() {}
	FItemAddResult(const int32 InItemQuantity, const int32 InQuantityAdded = 0) : AmountToGive(InItemQuantity), ActualAmountGiven(InQuantityAdded) {}
	
	UPROPERTY(BlueprintReadOnly, Category="Item Add Result")
	int32 AmountToGive;
	UPROPERTY(BlueprintReadOnly, Category="Item Add Result")
	int32 ActualAmountGiven;
	UPROPERTY(BlueprintReadOnly, Category="Item Add Result")
	EItemAddResult Result;
	UPROPERTY(BlueprintReadOnly, Category="Item Add Result")
	FText ErrorText;

	// Helpers
	static FItemAddResult AddedNone(const int32 InItemQuantity, const FText& ErrorText)
	{
		FItemAddResult AddedNoneResult(InItemQuantity);
		AddedNoneResult.Result = EItemAddResult::IAR_NoItemsAdded;
		AddedNoneResult.ErrorText = ErrorText;

		return AddedNoneResult;
	}
	static FItemAddResult AddedSome(const int32 InItemQuantity, const int32 ActualAmountGiven, const FText& ErrorText)
	{
		FItemAddResult AddedSomeResult(InItemQuantity, ActualAmountGiven);
		AddedSomeResult.Result = EItemAddResult::IAR_SomeItemsAdded;
		AddedSomeResult.ErrorText = ErrorText;

		return AddedSomeResult;
	}
	static FItemAddResult AddedAll(const int32 InItemQuantity)
	{
		FItemAddResult AddedAllResult(InItemQuantity, InItemQuantity);
		AddedAllResult.Result = EItemAddResult::IAR_AllItemsAdded;

		return AddedAllResult;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARCHIVESOFMEKDEMO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UItem;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category=Inventory)
	FItemAddResult TryAddItem(class UItem* Item);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	FItemAddResult TryAddItemFromClass(TSubclassOf<class UItem> ItemClass, const int32 Quantity);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool RemoveItem(class UItem* Item);

	UFUNCTION(BlueprintPure, Category=Inventory)
	bool HasItem(TSubclassOf<class UItem> ItemClass, const int32 Quantity = 1) const;

	UFUNCTION(BlueprintPure, Category=Inventory)
	UItem* FindItem(const class UItem* Item) const;

	UFUNCTION(BlueprintPure, Category=Inventory)
	UItem* FindItemByClass(TSubclassOf<class UItem> ItemClass) const;

	UFUNCTION(BlueprintPure, Category=Inventory)
	TArray<UItem*> FindItemsByClass(TSubclassOf<class UItem> ItemClass) const;

	UFUNCTION(BlueprintPure, Category=Inventory)
	float GetCurrentWeight() const;

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void SetWeightCapacity(const float NewWeightCapacity);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void SetCapacity(const int32 NewCapacity);

	int32 ConsumeItem(class UItem* Item);
	int32 ConsumeItem(class UItem* Item, const int32 Quantity = 1);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory")
	float WeightCapacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory", meta=(ClampMin=0, ClampMax=200))
	int32 Capacity;
	
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TArray<UItem*> Items;

private:
	FItemAddResult Int_TryAddItem(const class UItem* Item);

	/** Don't call Items.Add() directly, use this function instead as it handles replication and ownership */
	UItem* AddItem(const class UItem* Item);

public:
	UFUNCTION(BlueprintPure, Category="Inventory")
	FORCEINLINE float GetWeightCapacity() const { return WeightCapacity; }

	UFUNCTION(BlueprintPure, Category="Inventory")
	FORCEINLINE int32 GetCapacity() const { return Capacity; }

	UFUNCTION(BlueprintPure, Category="Inventory")
	FORCEINLINE TArray<UItem*> GetItems() const { return Items; }
};
