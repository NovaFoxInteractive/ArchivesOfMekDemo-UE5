// Copyright NovaFox Interactive L.L.C 2021


#include "World/Pickup.h"

#include "Camera/CameraComponent.h"
#include "Characters/CharacterBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Item.h"
#include "Core/Components/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Log.h"


APickup::APickup()
	: PickupDistance(150.f), WidgetDistanceAboveMesh(25.f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootComponent"));

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("PickupMesh");
	PickupMesh->SetupAttachment(GetRootComponent());

	Collision = CreateDefaultSubobject<USphereComponent>("Sphere");
	Collision->SetupAttachment(PickupMesh);
	Collision->SetSphereRadius(500.f);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &APickup::SphereOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &APickup::SphereEndOverlap);

	PromptWidget = CreateDefaultSubobject<UWidgetComponent>("PromptWidget");
	PromptWidget->SetupAttachment(GetRootComponent());
	PromptWidget->SetRelativeLocation(FVector(0.f, 0.f, WidgetDistanceAboveMesh));
}

void APickup::BeginPlay()
{
	Super::BeginPlay();
}

void APickup::SphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (const auto Character = Cast<ACharacterBase>(OtherActor))
	{
		CharacterRef = Character;
		PromptWidget->SetVisibility(true);
		bUpdateActor = true;
	}
}

void APickup::SphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (const auto Character = Cast<ACharacterBase>(OtherActor))
	{
		PromptWidget->SetVisibility(false);
		bUpdateActor = false;
		DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
}

bool APickup::PickupInput_Implementation()
{
	if(CharacterRef)
	{
		const auto AddItem = CharacterRef->GetInventoryComponent()->AddItem(Item, Amount);
		if(AddItem.bSuccess)
		{
			if(AddItem.Remainder <= 0)
				Destroy();
			else
				Amount = AddItem.Remainder;

			return true;
		}
		return false;
	}
	return false;
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bUpdateActor && CharacterRef)
	{
		UE_LOG(LogArchivesOfMek, Warning, TEXT("Updating Pickup"));
		auto WidgetLoc = PickupMesh->GetComponentLocation() + FVector(0.f, 0.f, WidgetDistanceAboveMesh);
		PromptWidget->SetWorldLocation(WidgetLoc);
		const auto TargetLocation = CharacterRef->GetPickupTraceCollision()->GetComponentLocation();

		const auto StartLocation = PromptWidget->GetComponentLocation();
		PromptWidget->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(StartLocation, CharacterRef->GetFollowCamera()->GetComponentLocation()));

		const auto Loc = GetActorLocation() - TargetLocation;
		const auto bCanPickup = Loc.Size() <= PickupDistance;
		PickupToggle(bCanPickup);

		if(bCanPickup)
			EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		else
			DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
}
