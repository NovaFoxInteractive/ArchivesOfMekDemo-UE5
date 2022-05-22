// Copyright NovaFox Interactive L.L.C 2021


#include "AI/EnemyControllerBase.h"

#include "AI/EnemyBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyControllerBase::AEnemyControllerBase()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
}

void AEnemyControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(!InPawn) return;

	AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn);
	if(Enemy)
		if(Enemy->GetBehaviorTree())
			BlackboardComponent->InitializeBlackboard(*Enemy->GetBehaviorTree()->BlackboardAsset);
}
