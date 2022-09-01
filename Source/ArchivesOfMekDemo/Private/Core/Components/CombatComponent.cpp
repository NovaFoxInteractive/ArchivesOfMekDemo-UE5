// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/Components/CombatComponent.h"

#include "Characters/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/Components/CustomMovementComponent.h"
#include "Item/Weapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCombatComponent::EquipWeaponFromIndex(int Index)
{
	if(EquippedWeapon)
		EquippedWeapon->Destroy();
	
	EquippedWeaponClass = Weapons[Index];

	// Spawn weapon and attach to character
	EquippedWeapon = Cast<AWeapon>(GetWorld()->SpawnActor(EquippedWeaponClass, &PlayerRef->GetMesh()->GetComponentTransform()));
}

void UCombatComponent::JumpAttackCheck()
{
	if (bAnimLoaded)
	{
		if (AnimNum == EAttackAnim::EAA_LJump || AnimNum == EAttackAnim::EAA_HJump)
		{
			if (!(PlayerRef->GetCharacterMovement()->IsFalling()))
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

void UCombatComponent::ResetAttack()
{
	if(bIsBlocking)
	{
		PreviousAttack = EAttackType::EAT_None;
		AttackCount = 0;
	}
}

void UCombatComponent::Attack(EAttackType Attack)
{
	CurrentAttackType = Attack;
	ComboLogic();

	if (!bAnimPlaying) // Check if in the middle of an attack
	{
		JumpAttackCheck();
		bAnimLoaded = false;
		const auto AttackAnim = GetAttackAnimation(AnimNum);
		OnAttack.Broadcast(AttackAnim);
	}
	else
		bAnimLoaded = true;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UCombatComponent::ComboLogic()
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
			if (ComboNumCheck())
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
			if (ComboNumCheck())
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
			if (ComboNumCheck())
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
			if (ComboNumCheck())
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

void UCombatComponent::Block()
{
	if (bCanBlock && !PlayerRef->GetCustomMovement()->GetIsDodging() && !(PlayerRef->GetCharacterMovement()->IsFalling()))
	{
		bIsBlocking = true;
		OnBlock.Broadcast();
	}
}

void UCombatComponent::StopBlock()
{
	bIsBlocking = false;
	OnStopBlock.Broadcast();
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UAnimationAsset* UCombatComponent::GetAttackAnimation(const EAttackAnim AnimationType)
{
	return AttackAnimations[EquippedWeaponClass].Animations[AnimationType];
}

