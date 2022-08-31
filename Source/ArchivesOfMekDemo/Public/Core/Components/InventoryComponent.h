// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Components/ActorComponent.h"
#include "Structs/InventoryItems.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateInventorySlot, int32, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateAmountOfSlots, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

USTRUCT()
struct FEmptySlotReturn
{
	GENERATED_BODY()
	
	bool bSuccess;
	int32 Index;

	FEmptySlotReturn() = default;
	FEmptySlotReturn(const bool bSuccess, const int32 Index) : bSuccess(bSuccess), Index(Index) {}
};

USTRUCT()
struct FAddItemReturn
{
	GENERATED_BODY()

	bool bSuccess;
	int32 Remainder;

	FAddItemReturn() = default;
	explicit FAddItemReturn(const bool bSuccess, const int32 Remainder = 0) : bSuccess(bSuccess), Remainder( Remainder) {}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARCHIVESOFMEKDEMO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TArray<FInventoryItems> InventorySlots;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	class ACharacterBase* PlayerRef;

	
public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	FAddItemReturn AddItem(TSubclassOf<AItem> Item, int32 Amount);
	
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnUpdateInventorySlot OnUpdateInventorySlot;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnUpdateAmountOfSlots OnUpdateAmountOfSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnUseItem OnUseItem;

	void SetupArray(ACharacterBase* Character);
	
	UFUNCTION(BlueprintCallable)
	void UseItem(int32 Index);
	
	UFUNCTION(BlueprintCallable)
	void RemoveItemAtIndex(int Index);
	
	UFUNCTION(BlueprintCallable)
	void DropItemAtIndex(int Index);

	void AddMoreSlots(int32 Amount);
private:
	FEmptySlotReturn CheckForEmptySlot();

	FEmptySlotReturn CheckForFreeSlot(TSubclassOf<AItem> Item);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE FInventoryItems GetItemDataAtIndex(const int32 Index) const { return InventorySlots[Index]; }

	UFUNCTION(BlueprintPure)
	bool InvalidItemAtIndex(const int32 Index) const { return IsValid(InventorySlots[Index].Item); }
};
