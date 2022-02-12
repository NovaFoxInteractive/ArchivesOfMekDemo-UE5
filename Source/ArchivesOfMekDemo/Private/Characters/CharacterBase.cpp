// Copyright NovaFox Interactive L.L.C 2021

#include "Characters/CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include "ArchivesOfMekDemoGameModeBase.h"

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

	MaxStamina = 100.f;
	CurrentStamina = MaxStamina;

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

	if (GameMode->Difficulty == EDifficulty::ED_Pro)
	{
		MaxStamina = CurrentHealth;
		if (CurrentStamina > MaxStamina)
			CurrentStamina = MaxStamina;
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
		}
		break;
	}
}

void ACharacterBase::ChangeStamina(float Val)
{
	CurrentStamina -= Val;
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