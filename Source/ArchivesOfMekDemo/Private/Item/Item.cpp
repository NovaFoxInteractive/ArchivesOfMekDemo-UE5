// Copyright NovaFox Interactive L.L.C 2021

#include "Item/Item.h"
#include "Core/Components/InventoryComponent.h"

AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

bool AItem::UseItem_Int()
{
	return UseItem();
}

bool AItem::UseItem_Implementation()
{
	return true;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
