// Copyright NovaFox Interactive L.L.C 2021


#include "World/Pickup.h"
#include "Characters/CharacterBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/ActorChannel.h"
#include "Item/Item.h"
#include "Core/Components/InteractionComponent.h"
#include "Core/Components/InventoryComponent.h"

#include "Log.h"

// Sets default values
APickup::APickup()
{
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);

	SetRootComponent(PickupMesh);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>("PickupInteractionComponent");
	InteractionComponent->SetInteractionTime(0.5f);
	InteractionComponent->SetInteractionDistance(200.f);
	InteractionComponent->SetInteractableNameText(FText::FromString("Pickup"));
	InteractionComponent->SetInteractableActionText(FText::FromString("Take"));
	InteractionComponent->OnInteract.AddDynamic(this, &APickup::OnTakePickup);
	InteractionComponent->SetupAttachment(PickupMesh);
}

void APickup::InitializePickup(const TSubclassOf<UItem> ItemClass, const int32 Quantity)
{
	if (ItemClass && Quantity > 0)
	{
		Item = NewObject<UItem>(this, ItemClass);
		Item->SetQuantity(Quantity);

		if(Item)
		{
			PickupMesh->SetStaticMesh(Item->GetPickupMesh());
		
			InteractionComponent->SetInteractableNameText(Item->GetItemDisplayName());

			Item->OnItemModified.AddDynamic(this, &APickup::OnItemModified);
		}
	}
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	if(ItemTemplate)
	{
		InitializePickup(ItemTemplate->GetClass(), ItemTemplate->GetQuantity());
	}

	InteractionComponent->RefreshWidget();
	AlignWithGround();
}

void APickup::OnItemModified()
{
	if(InteractionComponent)
		InteractionComponent->RefreshWidget();
}

#if WITH_EDITOR
void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if(PropertyName == GET_MEMBER_NAME_CHECKED(APickup, ItemTemplate))
		if(ItemTemplate)
			PickupMesh->SetStaticMesh(ItemTemplate->GetPickupMesh());
}
#endif

void APickup::OnTakePickup(ACharacterBase* Character)
{
	if(!Character)
	{
		UE_LOG(LogArchivesOfMek, Warning, TEXT("Pickup was taken but player was not valid."));
		return;
	}

	if(!IsPendingKillPending() && Item)
	{
		if (UInventoryComponent* PlayerInventory = Character->PlayerInventory)
		{
			const FItemAddResult AddResult = PlayerInventory->TryAddItem(Item);
			if(AddResult.ActualAmountGiven < Item->GetQuantity())
				Item->SetQuantity(Item->GetQuantity() - AddResult.ActualAmountGiven);
			else
				Destroy();
		}
	}
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

