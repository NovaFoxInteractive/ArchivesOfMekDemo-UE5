// Copyright NovaFox Interactive L.L.C 2021

#include "Core/Components/InventoryComponent.h"

#include "Camera/CameraComponent.h"
#include "Characters/CharacterBase.h"
#include "Characters/PlayerControllerBase.h"
#include "Engine/ActorChannel.h"
#include "Item/Item.h"
#include "Kismet/GameplayStatics.h"

UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}

FAddItemReturn UInventoryComponent::AddItem(TSubclassOf<AItem> Item, int32 Amount)
{
	const int32 MaxStackAmount = Item.GetDefaultObject()->GetItemData().MaxStackAmount;

	if(MaxStackAmount > 1)
	{
		const auto FreeSlot = CheckForFreeSlot(Item);
		if(FreeSlot.bSuccess)
		{
			const auto ItemData = GetItemDataAtIndex(FreeSlot.Index);
			int32 NewAmount = ItemData.Amount + Amount;

			if(NewAmount > MaxStackAmount)
			{
				NewAmount -= MaxStackAmount;
				InventorySlots[FreeSlot.Index] = FInventoryItems(Item, MaxStackAmount);
				OnUpdateInventorySlot.Broadcast(FreeSlot.Index);
				const auto AddItemReturn = AddItem(Item, NewAmount);
				return FAddItemReturn(true, AddItemReturn.Remainder);
			}

			InventorySlots[FreeSlot.Index] = FInventoryItems(Item, NewAmount);
			OnUpdateInventorySlot.Broadcast(FreeSlot.Index);
			return FAddItemReturn(true);
		}
	}	
	const auto EmptySlot = CheckForEmptySlot();
	if(EmptySlot.bSuccess)
	{
		if(Amount > MaxStackAmount)
		{
			InventorySlots[EmptySlot.Index] = FInventoryItems(Item, MaxStackAmount);
			OnUpdateInventorySlot.Broadcast(EmptySlot.Index);
			const auto AddItemReturn = AddItem(Item, Amount - MaxStackAmount);
			return FAddItemReturn(true, AddItemReturn.Remainder);
		}

		InventorySlots[EmptySlot.Index] = FInventoryItems(Item, Amount);
		OnUpdateInventorySlot.Broadcast(EmptySlot.Index);
		return FAddItemReturn(true);
	}
	return FAddItemReturn(false, Amount);
}

void UInventoryComponent::SetupArray(ACharacterBase* Character)
{
	if(const auto Controller = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		InventorySlots.SetNum(Controller->GetInventorySlots());
		PlayerRef = Character;
	}
}

void UInventoryComponent::UseItem(int32 Index)
{
	const auto Item = GetItemDataAtIndex(Index);
	
	if(Item.Amount < 1) return;
	
	auto ItemRef = Cast<AItem>(Item.Item);
	if(!ItemRef)
		ItemRef = Cast<AItem>(GetWorld()->SpawnActor(Item.Item));
	const auto Success = ItemRef->UseItem_Int();
	ItemRef->Destroy();
	
	if(!Success)
	{
		return;
	}
	
	RemoveItemAtIndex(Index);
	if(GetItemDataAtIndex(Index).Amount < 1)
		OnUseItem.Broadcast();
}

void UInventoryComponent::RemoveItemAtIndex(int Index)
{
	if(GetItemDataAtIndex(Index).Amount > 1)
	{
		InventorySlots[Index].Amount -= 1;
		OnUpdateInventorySlot.Broadcast(Index);
		return;
	}
	
	InventorySlots[Index] = FInventoryItems();
	OnUpdateInventorySlot.Broadcast(Index);
}

void UInventoryComponent::DropItemAtIndex(int Index)
{
	const auto Item = GetItemDataAtIndex(Index);
	const bool bCanBeDropped = Item.Item.GetDefaultObject()->GetItemData().bCanBeDropped;
	if(Item.Amount > 0)
	{
		for(int i = 0; i < Item.Amount; i++)
			RemoveItemAtIndex(Index);
			
		OnUseItem.Broadcast();
		OnUpdateInventorySlot.Broadcast(Index);
		
		//Only used if dropping item results in spawning a pickup
		if(bCanBeDropped)
		{
			FHitResult HitResult;
			const auto TraceStart = PlayerRef->GetPickupTraceCollision()->GetComponentLocation();
			const auto TraceEnd = (PlayerRef->GetPickupTraceCollision()->GetForwardVector() * 200.f) + TraceStart;
			
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(PlayerRef);
			
			FVector Loc;
			if(GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
				Loc = HitResult.Location;
			else
				Loc = TraceEnd;
			
			auto Pickup = Cast<APickup>(GetWorld()->SpawnActor(Item.Item.GetDefaultObject()->GetItemData().PickupMesh, &Loc));
			Pickup->UpdateAmount(Item.Amount);
			Pickup->GetPickupMesh()->SetSimulatePhysics(true);
		}
	}
}

void UInventoryComponent::AddMoreSlots(int32 Amount)
{
	if(const auto Controller = Cast<APlayerControllerBase>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		Controller->SetInventorySlots(Controller->GetInventorySlots() + Amount);
		InventorySlots.SetNum(Controller->GetInventorySlots());
		OnUpdateAmountOfSlots.Broadcast(Amount);
	}
}

FEmptySlotReturn UInventoryComponent::CheckForEmptySlot()
{
	bool bSuccess = false;
	int32 i = 0;
	for(auto& Item : InventorySlots)
	{
		if(!IsValid(Item.Item))
		{
			bSuccess = true;
			break;
		}
		i++;
	}
	return FEmptySlotReturn(bSuccess, i);
}

FEmptySlotReturn UInventoryComponent::CheckForFreeSlot(TSubclassOf<AItem> Item)
{
	bool bSuccess = false;
	int32 i = 0;
	for(auto& IItem : InventorySlots)
	{
		if(IItem.Item == Item && IItem.Amount < Item.GetDefaultObject()->GetItemData().MaxStackAmount)
		{
			bSuccess = true;
			break;
		}
		i++;
	}
	return FEmptySlotReturn(bSuccess, i);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


