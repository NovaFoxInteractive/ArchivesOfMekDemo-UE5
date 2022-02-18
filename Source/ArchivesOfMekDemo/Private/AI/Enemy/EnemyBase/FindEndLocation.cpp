// Copyright NovaFox Interactive L.L.C 2021


#include "AI/Enemy/EnemyBase/FindEndLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Enemy/EnemyBase/EnemyAiControllerBase.h"
//#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "AI/Enemy/EnemyBase/BlackboardKeys.h"

UFindEndLocation::UFindEndLocation(FObjectInitializer const& Object_Initializer)
{
	NodeName = TEXT("Find End Location");
}

EBTNodeResult::Type UFindEndLocation::ExecuteTask(UBehaviorTreeComponent& Owner_Comp, uint8* Node_Memory)
{
	// get AI controller and its NPC
	auto const Cont = Cast<AEnemyAIControllerBase>(Owner_Comp.GetAIOwner());

	// get empty actor location
	Cont->Get_Blackboard()->SetValueAsVector(BB_Keys::target_location, EndLoc);

	// finish with success
	FinishLatentTask(Owner_Comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
