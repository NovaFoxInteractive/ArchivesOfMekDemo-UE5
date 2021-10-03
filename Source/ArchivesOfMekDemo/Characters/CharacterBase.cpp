// Copyright NovaFox Interactive L.L.C 2021

#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../ArchivesOfMekDemoGameModeBase.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initialize CameraBoom Component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	// Initialize Camera Component
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GameMode = Cast<AArchivesOfMekDemoGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	// Initialize Variables
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	bIsDead = false;
	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
	BaseWalkSpeed = 600.f;
	SprintWalkSpeed = BaseWalkSpeed * 1.5f;

	bCanDodge = true;
	bCanAttack = true;
	bCanBlock = true;

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if dead
	if (CurrentHealth <= 0 && !bIsDead)
		INT_Death();
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
	if ((Controller != nullptr) && (Value != 0.f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintWalkSpeed;
}

void ACharacterBase::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
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
		PreviousAttack = EAttackType::EA_None;
		AttackCount = 0;
	}

	bIsAttacking = true;

	if (GetCharacterMovement()->IsFalling())
	{
		CurrentAttackType = EAttackType::EA_LightJumpAttack;
	}
	else
	{
		CurrentAttackType = EAttackType::EA_LightAttack;
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
		PreviousAttack = EAttackType::EA_None;
		AttackCount = 0;
	}

	bIsAttacking = true;

	if (GetCharacterMovement()->IsFalling())
	{
		CurrentAttackType = EAttackType::EA_HeavyJumpAttack;
	}
	else
	{
		CurrentAttackType = EAttackType::EA_HeavyAttack;
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
		case EAttackType::EA_LightAttack:
			AnimNum = 0; // Light
			PreviousAttack = CurrentAttackType;
			AttackCount++;
			break;

		case EAttackType::EA_HeavyAttack:
			AnimNum = 4; // Heavy
			PreviousAttack = CurrentAttackType;
			AttackCount++;
			break;

		case EAttackType::EA_LightJumpAttack:
			AnimNum = 8; // Light Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;

		case EAttackType::EA_HeavyJumpAttack:
			AnimNum = 9; // Heavy Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;
		}
		break;

	case EAttackType::EA_LightAttack:
		switch (CurrentAttackType)
		{
		case EAttackType::EA_LightAttack:
			if (INT_ComboNumCheck())
			{
				AnimNum = 2; // Light Light Light
				PreviousAttack = EAttackType::EA_None;
				AttackCount = 0;
			}
			else
			{
				AnimNum = 1; // Light Light
				PreviousAttack = CurrentAttackType;
				AttackCount++;
			}
			break;

		case EAttackType::EA_HeavyAttack:
			if (INT_ComboNumCheck())
			{
				AnimNum = 3; // Light Light Heavy
				PreviousAttack = EAttackType::EA_None;
				AttackCount = 0;
			}
			else
			{
				AnimNum = 4; // Heavy
				PreviousAttack = CurrentAttackType;
				AttackCount = 0;
				AttackCount++;
			}
			break;

		case EAttackType::EA_LightJumpAttack:
			AnimNum = 8; // Light Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;

		case EAttackType::EA_HeavyJumpAttack:
			AnimNum = 9; // Heavy Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;
		}
		break;

	case EAttackType::EA_HeavyAttack:
		switch (CurrentAttackType)
		{
		case EAttackType::EA_LightAttack:
			if (INT_ComboNumCheck())
			{
				AnimNum = 7; // Heavy Heavy Light
				PreviousAttack = EAttackType::EA_None;
				AttackCount = 0;
			}
			else
			{
				AnimNum = 0; // Light
				PreviousAttack = CurrentAttackType;
				AttackCount = 0;
				AttackCount++;
			}
			break;

		case EAttackType::EA_HeavyAttack:
			if (INT_ComboNumCheck())
			{
				AnimNum = 6; // Heavy Heavy Heavy
				PreviousAttack = EAttackType::EA_None;
				AttackCount = 0;
			}
			else
			{
				AnimNum = 5; // Heavy Heavy
				PreviousAttack = CurrentAttackType;
				AttackCount++;
			}
			break;

		case EAttackType::EA_LightJumpAttack:
			AnimNum = 8; // Light Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;

		case EAttackType::EA_HeavyJumpAttack:
			AnimNum = 9; // Heavy Jump
			PreviousAttack = CurrentAttackType;
			AttackCount = 0;
			break;
		}
		break;
	}
}

bool ACharacterBase::INT_ComboNumCheck()
{
	if (AttackCount > 1)
		return true;

	return false;
}

void ACharacterBase::JumpAttackCheck()
{
	if (bAnimLoaded)
	{
		if (AnimNum == 8 || AnimNum == 9)
		{
			if (!(GetCharacterMovement()->IsFalling()))
			{
				if (AnimNum == 8)
				{
					AnimNum = 0;
				}
				else
				{
					AnimNum = 4;
				}
			}
		}
	}
}

void ACharacterBase::INT_Death()
{
	bIsDead = true;
	bCanDodge = false;
	bCanAttack = false;
	bCanBlock = false;
	Death();
}

void ACharacterBase::INT_Dodge()
{
	if (bCanDodge && !bIsAttacking && !bIsDodging && !(GetCharacterMovement()->IsFalling()))
	{
		bIsDodging = true;
		Dodge();
	}
}

void ACharacterBase::INT_Block()
{
	if (bCanBlock && !bIsDodging && !(GetCharacterMovement()->IsFalling()))
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