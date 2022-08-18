// Copyright NovaFox Interactive L.L.C 2021

#include "Characters/CharacterBase.h"

#include "Log.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Core/ArchivesOfMekDemoGameModeBase.h"
#include "Core/Components/InventoryComponent.h"

#include "Core/Components/InteractionComponent.h"
#include "World/Pickup.h"

// Sets default values
ACharacterBase::ACharacterBase() :
	InteractionCheckFrequency(0.f), InteractionCheckDistance(1000.f)
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

	GameMode = Cast<AArchivesOfMekDemoGameModeBase>(UGameplayStatics::GetGameMode(AActor::GetWorld()));

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

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>("PlayerInventory");
	PlayerInventory->SetCapacity(30);
	PlayerInventory->SetWeightCapacity(75.f);
}

void ACharacterBase::UseItem(UItem* Item)
{
	if(!PlayerInventory || !Item || !PlayerInventory->FindItem(Item)) return;
	Item->Use_Int(this);

	if(Item->GetConsumable())
		PlayerInventory->ConsumeItem(Item, 1);
}

void ACharacterBase::DropItem(UItem* Item, const int32 Quantity)
{
	if(Item && PlayerInventory->FindItem(Item))
	{
		UE_LOG(LogArchivesOfMek, Warning, TEXT("Dropping item"));
		
		const int32 ItemQuantity = Item->GetQuantity();
		const int32 DroppedQuantity = PlayerInventory->ConsumeItem(Item, Quantity);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.bNoFail = true;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
		ensure(PickupClass);

		APickup* Pickup = GetWorld()->SpawnActor<APickup>(PickupClass, SpawnTransform, SpawnParameters);
		Pickup->InitializePickup(Item->GetClass(), DroppedQuantity);
	}
}

bool ACharacterBase::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float ACharacterBase::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(CurrentHealth - DamageAmount <= 0.f)
		CurrentHealth = 0.f;
	else
		CurrentHealth -= DamageAmount;
	
	return DamageAmount;
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

	if (GameMode)
		if (GameMode->GetDifficulty() == EDifficulty::ED_Pro)
		{
			UpdateStamina();
		}

	if (IsInteracting() || GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
		{
			PerformInteractionCheck();
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

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ACharacterBase::BeginInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ACharacterBase::EndInteract);
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

void ACharacterBase::ChangeStamina(float Val)
{
	CurrentStamina -= Val;
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

void ACharacterBase::UpdateStamina()
{
	MaxStamina = CurrentHealth;
	if (CurrentStamina > MaxStamina)
		CurrentStamina = MaxStamina;
}

void ACharacterBase::PerformInteractionCheck()
{
	if (GetController() == nullptr) return;

	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	FVector EyesLoc = GetCameraBoom()->GetComponentLocation();
	FRotator EyesRot = GetCameraBoom()->GetComponentRotation();

	const FVector TraceStart{ EyesLoc };
	const FVector TraceEnd{ (EyesRot.Vector() * InteractionCheckDistance) + TraceStart };
	FHitResult TraceHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		if (TraceHit.GetActor())
		{
			if (UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();
				if (InteractionComponent != GetInteractable() && Distance <= InteractionComponent->GetInteractionDistance())
					FoundNewInteractable(InteractionComponent);
				else if (Distance > InteractionComponent->GetInteractionDistance() && GetInteractable())
					CouldntFindInteractable();

				return;
			}
		}
	}

	CouldntFindInteractable();
}

void ACharacterBase::CouldntFindInteractable()
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);

		if (InteractionData.bInteractHeld)
		{
			EndInteract();
		}
	}

	InteractionData.ViewedInteractionComponent = nullptr;
}

void ACharacterBase::FoundNewInteractable(UInteractionComponent* Interactable)
{
	if(!Interactable) return;
	
	EndInteract();

	if(UInteractionComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}

	InteractionData.ViewedInteractionComponent = Interactable;
	Interactable->BeginFocus(this);
}

void ACharacterBase::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

void ACharacterBase::BeginInteract()
{
	PerformInteractionCheck();
	
	InteractionData.bInteractHeld = true;

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->BeginInteract(this);

		if (FMath::IsNearlyZero(Interactable->GetInteractionTime()))
			Interact();
		else
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &ACharacterBase::Interact, Interactable->GetInteractionTime(), false);
	}
}

void ACharacterBase::EndInteract()
{
	InteractionData.bInteractHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);

	if (UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void ACharacterBase::InteractCheckOverlap(UInteractionComponent* Interactable)
{
	FoundNewInteractable(Interactable);
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