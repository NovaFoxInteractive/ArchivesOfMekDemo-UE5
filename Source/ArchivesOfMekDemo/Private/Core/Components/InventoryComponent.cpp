// Copyright NovaFox Interactive L.L.C 2021

#include "Core/Components/InventoryComponent.h"
#include "Engine/ActorChannel.h"
#include "Item/Item.h"

#define LOCTEXT_NAMESPACE "Inventory"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	SetIsReplicated(true);
}

FItemAddResult UInventoryComponent::TryAddItem(UItem* Item)
{
	return Int_TryAddItem(Item);
}

FItemAddResult UInventoryComponent::TryAddItemFromClass(const TSubclassOf<UItem> ItemClass, const int32 Quantity)
{
	UItem* Item = NewObject<UItem>(GetOwner(), ItemClass);
	Item->SetQuantity(Quantity);
	return Int_TryAddItem(Item);
}

bool UInventoryComponent::RemoveItem(UItem* Item)
{
	if(GetOwner())
	{
		if(Item)
		{
			Items.RemoveSingle(Item);
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::HasItem(const TSubclassOf<UItem> ItemClass, const int32 Quantity) const
{
	if(const UItem* ItemToFind =  FindItemByClass(ItemClass))
		return ItemToFind->GetQuantity() >= Quantity;
	return false;
}

UItem* UInventoryComponent::FindItem(const UItem* Item) const
{
	if(Item)
		for(auto& InvItem : Items)
			if(InvItem && InvItem->GetClass() == Item->GetClass())
				return InvItem;
	return nullptr;
}

UItem* UInventoryComponent::FindItemByClass(const TSubclassOf<UItem> ItemClass) const
{
	for(auto& InvItem : Items)
		if(InvItem && InvItem->GetClass() == ItemClass)
			return InvItem;
	return nullptr;
}

TArray<UItem*> UInventoryComponent::FindItemsByClass(const TSubclassOf<UItem> ItemClass) const
{
	TArray<UItem*> ItemsOfClass;
	
	for(auto& InvItem : Items)
		if(InvItem && InvItem->GetClass()->IsChildOf(ItemClass))
			ItemsOfClass.Add(InvItem);
	return ItemsOfClass;
}

float UInventoryComponent::GetCurrentWeight() const
{
	float Weight = 0.f;
	for(auto& Item : Items)
		if(Item)
			Weight += Item->GetStackWeight();
	return Weight;
}

void UInventoryComponent::SetWeightCapacity(const float NewWeightCapacity)
{
	WeightCapacity = NewWeightCapacity;
}

void UInventoryComponent::SetCapacity(const int32 NewCapacity)
{
	Capacity = NewCapacity;
}

int32 UInventoryComponent::ConsumeItem(UItem* Item)
{
	if(Item)
		return ConsumeItem(Item, Item->GetQuantity());
	return 0;
}

int32 UInventoryComponent::ConsumeItem(UItem* Item, const int32 Quantity)
{
	if(GetOwner() && Item)
	{
		const int32 RemoveQuantity = FMath::Min(Quantity, Item->GetQuantity());
		ensure(!(Item->GetQuantity() - RemoveQuantity < 0));

		Item->SetQuantity(Item->GetQuantity() <= RemoveQuantity);
		if(Item->GetQuantity() <= 0)
			RemoveItem(Item);

		return RemoveQuantity;
	}
	return 0;
}

FItemAddResult UInventoryComponent::Int_TryAddItem(const UItem* Item)
{
	if(GetOwner())
	{
		const int32 AddAmount = Item->GetQuantity();

		if(Items.Num() + 1 > GetCapacity())
			return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryCapacityFullText", "Couldn't add item to Inventory. Inventory is full"));

		if(!FMath::IsNearlyZero(Item->GetWeight()))
			if(GetCurrentWeight() + Item->GetWeight() > GetWeightCapacity())
				return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryTooMuchWeightText", "Couldn't add item to Inventory. Carrying too much weight"));

		if(Item->GetIsStackable())
		{
			ensure(Item->GetQuantity() <= Item->GetMaxStackSize());
			if(UItem* ExistingItem = FindItem(Item))
			{
				if(ExistingItem->GetQuantity() < ExistingItem->GetMaxStackSize())
				{
					const int32 CapacityMaxAddAmount = ExistingItem->GetMaxStackSize() - ExistingItem->GetQuantity();
					int32 ActualAddAmount = FMath::Min(AddAmount, CapacityMaxAddAmount);

					FText ErrorText = LOCTEXT("InventoryErrorText", "Couldn't add all of the items to your inventory.");
					if(!FMath::IsNearlyZero(Item->GetWeight()))
					{
						const int32 WeightMaxAddAmount = FMath::FloorToInt((WeightCapacity - GetCurrentWeight()) / Item->GetWeight());
						ActualAddAmount = FMath::Min(ActualAddAmount, WeightMaxAddAmount);

						if(ActualAddAmount < AddAmount)
							ErrorText = FText::Format(LOCTEXT("InventoryTooMuchWeightText", "Couldn't add entire stack of {ItemName} to Inventory."), Item->GetItemDisplayName());
					}
					else if (ActualAddAmount < AddAmount)
					{
						ErrorText = FText::Format(LOCTEXT("InventoryCapacityFullText", "Couldn't add entire stack of {ItemName} to Inventory. Inventory was full."), Item->GetItemDisplayName());
					}

					if(ActualAddAmount <= 0)
						return FItemAddResult::AddedNone(AddAmount, LOCTEXT("InventoryErrorText", "Couldn't add item to inventory."));

					ExistingItem->SetQuantity(ExistingItem->GetQuantity() + ActualAddAmount);
					ensure(ExistingItem->GetQuantity() <= ExistingItem->GetMaxStackSize());

					if (ActualAddAmount < AddAmount)
						return FItemAddResult::AddedSome(AddAmount, ActualAddAmount, ErrorText);
					else
						return FItemAddResult::AddedAll(AddAmount);
				}
				else
				{
					FItemAddResult::AddedNone(AddAmount, FText::Format(LOCTEXT("InventoryFullStackText", "Couldn't add {ItemName}. You already have a full stack of this Item"), Item->GetItemDisplayName()));
				}
			}
			else
			{
				AddItem(Item);
				return FItemAddResult::AddedAll(Item->GetQuantity());
			}
		}
		else
		{
			ensure(Item->GetQuantity() == 1);
			
			AddItem(Item);
			return FItemAddResult::AddedAll(Item->GetQuantity());
		}
	}
	
	check(false);
	return FItemAddResult::AddedNone(-1, LOCTEXT("ErrorMessage", ""));
}

UItem* UInventoryComponent::AddItem(const UItem* Item)
{
	const auto& OwnerRef = GetOwner();
	if(OwnerRef)
	{
		UItem* NewItem = NewObject<UItem>(OwnerRef, Item->GetClass());
		NewItem->SetQuantity(Item->GetQuantity());
		NewItem->SetOwningInventory(this);
		NewItem->AddedToInventory(this);
		Items.Add(NewItem);

		return NewItem;
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE