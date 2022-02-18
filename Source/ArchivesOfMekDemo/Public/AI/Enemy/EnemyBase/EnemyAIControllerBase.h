// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class ARCHIVESOFMEKDEMO_API AEnemyAIControllerBase : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyAIControllerBase(FObjectInitializer const& Object_Initializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* const PawnToPossess) override;
private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI/Enemy", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* Behavior_Tree_Component;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI/Enemy", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BTree;

	UPROPERTY()
	class UBlackboardComponent* BBoard;
public:
	FORCEINLINE class UBlackboardComponent* Get_Blackboard() const { return BBoard; }
};
