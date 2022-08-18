// Copyright NovaFox Interactive L.L.C 2021


#include "Core/Components/HealthComponent.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
	: MaxHealth(100.f), MinHealth(0.f), CurrentHealth(MaxHealth),
		bIsDead(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UHealthComponent::UpdateHealth(const float Amount)
{
	if(Amount < 0)
		CurrentHealth = FMath::Clamp(CurrentHealth - Amount, MinHealth, MaxHealth);
	else
		CurrentHealth = FMath::Clamp(CurrentHealth + Amount, MinHealth, MaxHealth);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

