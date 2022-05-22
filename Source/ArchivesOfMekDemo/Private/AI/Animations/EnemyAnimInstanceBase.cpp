// Copyright NovaFox Interactive L.L.C 2021


#include "AI/Animations/EnemyAnimInstanceBase.h"

#include "AI/EnemyBase.h"

void UEnemyAnimInstanceBase::UpdateAnimationProperties(float DeltaTime)
{
	if(!Enemy)
		Enemy = Cast<AEnemyBase>(TryGetPawnOwner());

	if(Enemy)
	{
		FVector Velocity{Enemy->GetVelocity()};
		Velocity.Z = 0.f;
		Speed = Velocity.Size();
	}
}
