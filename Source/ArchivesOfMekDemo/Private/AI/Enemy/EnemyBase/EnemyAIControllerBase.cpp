// Copyright NovaFox Interactive L.L.C 2021


#include "AI/Enemy/EnemyBase/EnemyAIControllerBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Characters/CharacterBase.h"


AEnemyAIControllerBase::AEnemyAIControllerBase(FObjectInitializer const& Object_Initializer)
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree>Obj(TEXT("BehaviorTree'/Game/_Game/AI/Enemies/BaseBlueprints/EnemyBse_BT.EnemyBse_BT'"));
	if (Obj.Succeeded())
	{
		BTree = Obj.Object;
	}
	Behavior_Tree_Component = Object_Initializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	BBoard = Object_Initializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));
}

void AEnemyAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BTree);
	Behavior_Tree_Component->StartTree(*BTree);
}

void AEnemyAIControllerBase::OnPossess(APawn* const PawnToPossess)
{
	Super::OnPossess(PawnToPossess);
	if (BBoard)
	{
		BBoard->InitializeBlackboard(*BTree->BlackboardAsset);
	}
}
