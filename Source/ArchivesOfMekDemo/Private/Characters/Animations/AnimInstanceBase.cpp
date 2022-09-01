// Copyright NovaFox Interactive L.L.C 2021

#include "Characters/Animations/AnimInstanceBase.h"
#include "Characters/CharacterBase.h"
#include "Core/Components/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

UAnimInstanceBase::UAnimInstanceBase() :
	MovementSpeed(0.f), bIsInAir(false), bIsCrouching(false), bIsBlocking(false)
{

}

void UAnimInstanceBase::NativeInitializeAnimation()
{
	if (!Pawn)
	{
		Pawn = Cast<ACharacterBase>(UGameplayStatics::GetPlayerPawn(this, 0));
	}
}

void UAnimInstanceBase::UpdateAnimationProperties(float DeltaTime)
{
	if (!Pawn)
	{
		Pawn = Cast<ACharacterBase>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	if (Pawn)
	{
		const FVector Speed = Pawn->GetVelocity();
		const FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
		bIsCrouching = Pawn->GetMovementComponent()->IsCrouching();
		bIsBlocking = Pawn->GetCombatComponent()->GetIsBlocking();
	}
}