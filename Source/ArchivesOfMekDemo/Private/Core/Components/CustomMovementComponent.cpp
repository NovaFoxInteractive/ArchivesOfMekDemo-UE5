


#include "Core/Components/CustomMovementComponent.h"

#include "Characters/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UCustomMovementComponent::UCustomMovementComponent() :
	SprintSpeed(550.f), WalkSpeed(275.f),
	MaxStamina(100.f), MinStamina(0.f), CurrentStamina(MaxStamina),
	StaminaDepletionRate(0.1f), StaminaRecoveryRate(0.03f), StaminaRecoveryDelayRate(5.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCustomMovementComponent::Initialize(ACharacterBase* Character)
{
	PlayerRef = Character;
	ensure(PlayerRef);

	PlayerRef->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void UCustomMovementComponent::StartSprint()
{
	if (CurrentStamina <= MinStamina && PlayerRef->GetVelocity().Size() > 10.f && !PlayerRef) return;

	GetWorld()->GetTimerManager().ClearTimer(StaminaRecoveryTimer);
	PlayerRef->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	GetWorld()->GetTimerManager().SetTimer(SprintDepletionTimer, this, &UCustomMovementComponent::SprintTimer, StaminaDepletionRate, true);
}

void UCustomMovementComponent::StopSprint()
{
	GetWorld()->GetTimerManager().ClearTimer(SprintDepletionTimer);

	PlayerRef->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	GetWorld()->GetTimerManager().SetTimer(StaminaRecoveryDelayTimer, this, &UCustomMovementComponent::StaminaRecoveryDelay, StaminaRecoveryDelayRate, false);
}

void UCustomMovementComponent::ProModeStaminaUpdate(const float CurrentHealth)
{
	MaxStamina = CurrentHealth;
	if (CurrentStamina > MaxStamina)
		CurrentStamina = MaxStamina;
}

// Called when the game starts
void UCustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCustomMovementComponent::SprintTimer()
{
	if(PlayerRef->GetVelocity().Size() < 10.f)
		StopSprint();

	CurrentStamina = FMath::Clamp(CurrentStamina - 1.f, MinStamina, MaxStamina);

	if(CurrentStamina <= MinStamina)
		StopSprint();
}

void UCustomMovementComponent::StaminaRecoveryDelay()
{
	if(!(GetWorld()->GetTimerManager().IsTimerActive(SprintDepletionTimer)))
		GetWorld()->GetTimerManager().SetTimer(StaminaRecoveryTimer, this, &UCustomMovementComponent::StaminaRecovery, StaminaRecoveryRate, true);
}

void UCustomMovementComponent::StaminaRecovery()
{
	CurrentStamina = FMath::Clamp(CurrentStamina + 1.f, MinStamina, MaxStamina);

	if (CurrentStamina >= MaxStamina)
		GetWorld()->GetTimerManager().ClearTimer(StaminaRecoveryTimer);
}

// Called every frame
void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

