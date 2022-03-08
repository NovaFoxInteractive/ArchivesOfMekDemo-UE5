// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemModified);

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Epic UMETA(DisplayName = "Epic"),
	EIR_Legendary UMETA(DisplayName = "Legendary")
};

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class ARCHIVESOFMEKDEMO_API UItem : public UObject
{
	GENERATED_BODY()
	
protected:

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
public:
	UItem();

	UPROPERTY(BlueprintAssignable)
	FOnItemModified OnItemModified;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowProtectedAccess="true"))
	class UStaticMesh* PickupMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowProtectedAccess="true"))
	class UTexture2D* Thumbnail;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item", meta=(AllowProtectedAccess="true"))
	FText ItemDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item", meta=(MultiLine="true"), meta=(AllowProtectedAccess="true"))
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item", meta=(AllowProtectedAccess="true"))
	FText UseActionText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowProtectedAccess="true"))
	EItemRarity Rarity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowProtectedAccess="true"))
	float Weight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(AllowProtectedAccess="true"))
	bool bStackable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(EditCondition = bStackable), meta=(AllowProtectedAccess="true"))
	int32 MaxStackSize;

	/*
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Item", meta=(AllowProtectedAccess="true"))
	TSubclassOf<class UItemToolTip> ItemToolTip;
	*/

	UPROPERTY(EditAnywhere, Category="Item", meta=(UIMin = 1 , EditCondition = bStackable), meta=(AllowProtectedAccess="true"))
	int32 Quantity;

	UPROPERTY()
	class UInventoryComponent* OwningInventory;

	UPROPERTY()
	int32 RepKey;

public:
	/*
	FORCEINLINE UTexture2D* GetThumbnail() const { return Thumbnail; }
	FORCEINLINE FText GetItemDescription() const { return ItemDescription; }
	FORCEINLINE FText GetUseActionText() const { return UseActionText; }
	FORCEINLINE EItemRarity GetRarity() const { return Rarity; }
	FORCEINLINE int32 GetQuantity() const { return Quantity; }
    */
	FORCEINLINE UStaticMesh* GetPickupMesh() const { return PickupMesh; }
	FORCEINLINE FText GetItemDisplayName() const { return ItemDisplayName; }
	FORCEINLINE int32 GetMaxStackSize() const { return MaxStackSize; }
	FORCEINLINE float GetWeight() const	{ return Weight; }
	FORCEINLINE bool GetIsStackable() const { return bStackable; }
	
	FORCEINLINE int32 GetRepKey() const { return RepKey; }

	UFUNCTION(BlueprintCallable, Category="Item")
	FORCEINLINE float GetStackWeight() const { return Quantity * Weight; }
	UFUNCTION(BlueprintPure, Category="Item")
	virtual bool ShouldShowInInventory() const;
	UFUNCTION(BlueprintCallable, Category="Item")
	void SetQuantity(const int32 NewQuantity);
	UFUNCTION(BlueprintPure, Category="Item")
	FORCEINLINE int32 GetQuantity() const {return Quantity;}

	void SetOwningInventory(class UInventoryComponent* InventoryComponent);
	
	virtual void Use(class ACharacterBase* Character);
	virtual void AddedToInventory(class UInventoryComponent* Inventory);

};
