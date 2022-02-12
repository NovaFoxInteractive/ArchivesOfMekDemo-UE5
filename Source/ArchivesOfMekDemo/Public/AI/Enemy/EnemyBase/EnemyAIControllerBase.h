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
	AEnemyAIControllerBase(FObjectInitializer const& object_initializer = FObjectInitializer::Get());
	void BeginPlay() override;
	void OnPossess(APawn* const pawn) override;
private:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI/Enemy", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* behavior_tree_component;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI/Enemy", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* btree;

	class UBlackboardComponent* blackboard;
public:
	FORCEINLINE class UBlackboardComponent* get_blackboard() const { return blackboard; }
};
