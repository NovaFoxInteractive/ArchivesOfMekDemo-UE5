// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class ARCHIVESOFMEKDEMO_API AEnemyControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyControllerBase();

	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true" ))
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true" ))
	class UBehaviorTreeComponent* BehaviorTreeComponent;
public:
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorTreeComponent() const { return BehaviorTreeComponent; }
	
};
