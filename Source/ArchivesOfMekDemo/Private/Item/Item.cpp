// Copyright NovaFox Interactive L.L.C 2021

#include "Item/Item.h"
#include "Core/Components/InventoryComponent.h"

#define LOCTEXT_NAMESPACE "Item"

#if WITH_EDITOR
void UItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if(ChangedPropertyName == GET_MEMBER_NAME_CHECKED(UItem, Quantity))
	{
		Quantity = FMath::Clamp(Quantity, 1, bStackable ? MaxStackSize : 1);
	}
}
#endif

UItem::UItem() :
ItemDisplayName(LOCTEXT("ItemName", "Item")),
UseActionText(LOCTEXT("ItemUseActionText", "Use")),
Weight(0.f),
bStackable(true), MaxStackSize(2), Quantity(1),
RepKey(0)
{
	
}

bool UItem::ShouldShowInInventory() const
{
	return true;
}

void UItem::SetQuantity(const int32 NewQuantity)
{
	if(NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, bStackable ? MaxStackSize : 1);
	}
}

void UItem::SetOwningInventory(UInventoryComponent* InventoryComponent)
{
	OwningInventory = InventoryComponent;
}

void UItem::Use(ASurvivalCharacter* Character)
{
}

void UItem::AddedToInventory(UInventoryComponent* Inventory)
{
}

#undef LOCTEXT_NAMESPACE

