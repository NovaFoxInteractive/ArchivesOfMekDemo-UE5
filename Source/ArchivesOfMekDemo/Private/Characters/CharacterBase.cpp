// Copyright NovaFox Interactive L.L.C 2021

#include "Characters/CharacterBase.h"

#include "Log.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Core/ArchivesOfMekDemoGameModeBase.h"
#include "Core/Components/CombatComponent.h"
#include "Core/Components/CustomMovementComponent.h"
#include "Core/Components/HealthComponent.h"
#include "Core/Components/InventoryComponent.h"

#include "World/Pickup.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize CameraBoom Component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Initialize Camera Component
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PickupTraceCollision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupTraceCollision"));
	PickupTraceCollision->SetupAttachment(GetMesh());
	PickupTraceCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Initialize Stat Components
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	CustomMovementComponent = CreateDefaultSubobject<UCustomMovementComponent>(TEXT("CustomMovementComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent")); //
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	CombatComponent->Initialize(this);
	
	// Initialize Variables
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HealthComponent->UpdateHealth(DamageAmount);
	return DamageAmount;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AArchivesOfMekDemoGameModeBase>(UGameplayStatics::GetGameMode(AActor::GetWorld()));
	CustomMovementComponent->Initialize(this);
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if dead
	if (HealthComponent->Died())
		INT_Death();

	if (GameMode)
		if (GameMode->GetDifficulty() == EDifficulty::ED_Pro)
		{
			CustomMovementComponent->ProModeStaminaUpdate(HealthComponent->GetCurrentHealth());
		}
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Setup Axis Inputs
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);
	// Setup Action Inputs
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACharacterBase::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACharacterBase::StopSprint);
	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &ACharacterBase::INT_LightAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ACharacterBase::INT_HeavyAttack);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ACharacterBase::Dodge);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &ACharacterBase::Block);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &ACharacterBase::StopBlock);
}

void ACharacterBase::MoveForward(float Value)
{
	if ((GetController() != nullptr) && (Value != 0.f))
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if ((GetController() != nullptr) && (Value != 0.f))
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::Sprint()
{
	CustomMovementComponent->StartSprint();
}

void ACharacterBase::StopSprint()
{
	CustomMovementComponent->StopSprint();
}

void ACharacterBase::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::INT_LightAttack()
{
	if (CombatComponent->GetCanAttack()) return; // Check if Can Attack

	CombatComponent->ResetAttack();

	CombatComponent->SetIsAttacking();

	if (GetCharacterMovement()->IsFalling())
		CombatComponent->Attack(EAttackType::EAT_LightJumpAttack);
	else
		CombatComponent->Attack(EAttackType::EAT_LightAttack);
}

void ACharacterBase::INT_HeavyAttack()
{
	if (CombatComponent->GetCanAttack()) return; // Check if Can Attack

	CombatComponent->ResetAttack();
	
	CombatComponent->SetIsAttacking();

	if (GetCharacterMovement()->IsFalling())
		CombatComponent->Attack(EAttackType::EAT_HeavyJumpAttack);
	else
		CombatComponent->Attack(EAttackType::EAT_HeavyAttack);
}

void ACharacterBase::INT_Death()
{
	HealthComponent->SetIsDead();
	CustomMovementComponent->SetCanDodge(false);
	CombatComponent->SetValuesFalse();
	GetCharacterMovement()->DisableMovement();
	Death();
}

void ACharacterBase::Dodge()
{
	CustomMovementComponent->Dodge();
}

void ACharacterBase::Block()
{
	CombatComponent->Block();
}

void ACharacterBase::StopBlock()
{
	CombatComponent->StopBlock();
}