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

	// Initialize Variables
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;
	
	bCanAttack = true;
	bCanBlock = true;

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
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
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ACharacterBase::INT_Dodge);
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &ACharacterBase::INT_Block);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &ACharacterBase::INT_StopBlock);
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
	if (!bCanAttack || bAnimLoaded) return; // Check if Can Attack

	if (bIsBlocking)
	{
		PreviousAttack = EAttackType::EAT_None;
		AttackCount = 0;
	}

	bIsAttacking = true;

	if (GetCharacterMovement()->IsFalling())
	{
		CurrentAttackType = EAttackType::EAT_LightJumpAttack;
	}
	else
	{
		CurrentAttackType = EAttackType::EAT_LightAttack;
	}

	INT_ComboLogic();

	if (!bAnimPlaying) // Check if in the middle of an attack
		Attack();
	else
		bAnimLoaded = true;
}

void ACharacterBase::INT_HeavyAttack()
{
	if (!bCanAttack || bAnimLoaded) return; // Check if Can Attack

	if (bIsBlocking)
	{
		PreviousAttack = EAttackType::EAT_None;
		AttackCount = 0;
	}

	bIsAttacking = true;

	if (GetCharacterMovement()->IsFalling())
	{
		CurrentAttackType = EAttackType::EAT_HeavyJumpAttack;
	}
	else
	{
		CurrentAttackType = EAttackType::EAT_HeavyAttack;
	}

	INT_ComboLogic();

	if (!bAnimPlaying) // Check if in the middle of an attack
		Attack();
	else
		bAnimLoaded = true;
}

void ACharacterBase::INT_ComboLogic()
{
	switch (PreviousAttack)
	{
	default:
		switch (CurrentAttackType)
		{
		case EAttackType::EAT_LightAttack:
			AnimNum = EAttackAnim::EAA_L; // Light
			PreviousAttack = CurrentAttackType;
			AttackCount++;
			break;

		case EAttackType::EAT_HeavyAttack:
			AnimNum = EAttackAnim::EAA_H; // Heavy
			PreviousAttack = CurrentAttackType;
			AttackCount++;
			break;

		case EAttackType::EAT_LightJumpAttack:
			AnimNum = EAttackAnim::EAA_LJump; // Light Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;

		case EAttackType::EAT_HeavyJumpAttack:
			AnimNum = EAttackAnim::EAA_HJump; // Heavy Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;
		default:
			break;
		}
		break;

	case EAttackType::EAT_LightAttack:
		switch (CurrentAttackType)
		{
		case EAttackType::EAT_LightAttack:
			if (INT_ComboNumCheck())
			{
				AnimNum = EAttackAnim::EAA_LLL; // Light Light Light
				PreviousAttack = EAttackType::EAT_None;
				AttackCount = 0;
			}
			else
			{
				AnimNum = EAttackAnim::EAA_LL; // Light Light
				PreviousAttack = CurrentAttackType;
				AttackCount++;
			}
			break;

		case EAttackType::EAT_HeavyAttack:
			if (INT_ComboNumCheck())
			{
				AnimNum = EAttackAnim::EAA_LLH; // Light Light Heavy
				PreviousAttack = EAttackType::EAT_None;
				AttackCount = 0;
			}
			else
			{
				AnimNum = EAttackAnim::EAA_H; // Heavy
				PreviousAttack = CurrentAttackType;
				AttackCount = 0;
				AttackCount++;
			}
			break;

		case EAttackType::EAT_LightJumpAttack:
			AnimNum = EAttackAnim::EAA_LJump; // Light Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;

		case EAttackType::EAT_HeavyJumpAttack:
			AnimNum = EAttackAnim::EAA_HJump; // Heavy Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;
		default:
			break;
		}
		break;

	case EAttackType::EAT_HeavyAttack:
		switch (CurrentAttackType)
		{
		case EAttackType::EAT_LightAttack:
			if (INT_ComboNumCheck())
			{
				AnimNum = EAttackAnim::EAA_HHL; // Heavy Heavy Light
				PreviousAttack = EAttackType::EAT_None;
				AttackCount = 0;
			}
			else
			{
				AnimNum = EAttackAnim::EAA_L; // Light
				PreviousAttack = CurrentAttackType;
				AttackCount = 0;
				AttackCount++;
			}
			break;

		case EAttackType::EAT_HeavyAttack:
			if (INT_ComboNumCheck())
			{
				AnimNum = EAttackAnim::EAA_HHH; // Heavy Heavy Heavy
				PreviousAttack = EAttackType::EAT_None;
				AttackCount = 0;
			}
			else
			{
				AnimNum = EAttackAnim::EAA_HH; // Heavy Heavy
				PreviousAttack = CurrentAttackType;
				AttackCount++;
			}
			break;

		case EAttackType::EAT_LightJumpAttack:
			AnimNum = EAttackAnim::EAA_LJump; // Light Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;

		case EAttackType::EAT_HeavyJumpAttack:
			AnimNum = EAttackAnim::EAA_HJump; // Heavy Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;
		default:
			break;
		}
		break;
	}
}

bool ACharacterBase::INT_ComboNumCheck() const
{
	if (AttackCount > 1)
		return true;

	return false;
}

void ACharacterBase::JumpAttackCheck()
{
	if (bAnimLoaded)
	{
		if (AnimNum == EAttackAnim::EAA_LJump || AnimNum == EAttackAnim::EAA_HJump)
		{
			if (!(GetCharacterMovement()->IsFalling()))
			{
				if (AnimNum == EAttackAnim::EAA_LJump)
				{
					AnimNum = EAttackAnim::EAA_L;
				}
				else
				{
					AnimNum = EAttackAnim::EAA_H;
				}
			}
		}
	}
}

void ACharacterBase::INT_Death()
{
	HealthComponent->SetIsDead();
	CustomMovementComponent->SetCanDodge(false);
	bCanAttack = false;
	bCanBlock = false;
	GetCharacterMovement()->DisableMovement();
	Death();
}

void ACharacterBase::INT_Dodge()
{
	CustomMovementComponent->Dodge();
}

void ACharacterBase::INT_Block()
{
	if (bCanBlock && !CustomMovementComponent->GetIsDodging() && !(GetCharacterMovement()->IsFalling()))
	{
		bIsBlocking = true;
		Block();
	}
}

void ACharacterBase::INT_StopBlock()
{
	bIsBlocking = false;
	StopBlock();
}