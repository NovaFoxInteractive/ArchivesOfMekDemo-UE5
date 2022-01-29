// Copyright NovaFox Interactive L.L.C 2021


#include "FindEndLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyAiControllerBase.h"
//#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BlackboardKeys.h"

UFindEndLocation::UFindEndLocation(FObjectInitializer const& object_initializer)
{
	NodeName = TEXT("Find End Location");
}

EBTNodeResult::Type UFindEndLocation::ExecuteTask(UBehaviorTreeComponent& owner_comp, uint8* node_memory)
{
	// get AI controller and its NPC
	auto const cont = Cast<AEnemyAIControllerBase>(owner_comp.GetAIOwner());

	// get empty actor location
	cont->get_blackboard()->SetValueAsVector(bb_keys::target_location, endLoc);

	// finish with success
	FinishLatentTask(owner_comp, EBTNodeResult::Succeeded);
	return EBTNodeResult::Succeeded;
}
