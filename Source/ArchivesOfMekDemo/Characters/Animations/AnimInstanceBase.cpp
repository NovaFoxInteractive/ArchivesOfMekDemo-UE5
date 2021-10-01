// Copyright NovaFox Interactive L.L.C 2021

#include "AnimInstanceBase.h"
#include "../CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimInstanceBase::NativeInitializeAnimation()
{
	if (!Pawn)
	{
		Pawn = Cast<ACharacterBase>(UGameplayStatics::GetPlayerPawn(this, 0));
	}
}

void UAnimInstanceBase::UpdateAnimationProperties()
{
	if (!Pawn)
	{
		Pawn = Cast<ACharacterBase>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
		bIsCrouching = Pawn->GetMovementComponent()->IsCrouching();
		bIsBlocking = Pawn->GetIsBlocking();
	}
}