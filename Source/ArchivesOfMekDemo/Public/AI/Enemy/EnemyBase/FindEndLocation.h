// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "UObject/UObjectGlobals.h"
#include "FindEndLocation.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ARCHIVESOFMEKDEMO_API UFindEndLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UFindEndLocation(FObjectInitializer const& Object_Initializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Owner_Comp, uint8* Node_Memory) override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FVector EndLoc;
};
