// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARCHIVESOFMEKDEMO_API UCustomMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	// Player Reference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class ACharacterBase* PlayerRef;

	// Movement Values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values|Sprint", meta=(AllowPrivateAccess="true"))
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values|Sprint", meta=(AllowPrivateAccess="true"))
	float WalkSpeed;
	
	// Stamina
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Values|Sprint", meta = (AllowPrivateAccess = "true"))
	float MaxStamina;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Values|Sprint", meta = (AllowPrivateAccess = "true"))
	float MinStamina;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Values|Sprint", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina;

	// Rates
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Values|Sprint", meta=(AllowPrivateAccess="true"))
	float StaminaDepletionRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Values|Sprint", meta=(AllowPrivateAccess="true"))
	float StaminaRecoveryRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Values|Sprint", meta=(AllowPrivateAccess="true"))
	float StaminaRecoveryDelayRate;

	// Timer Handles
	FTimerHandle SprintDepletionTimer;

	FTimerHandle StaminaRecoveryDelayTimer;

	FTimerHandle StaminaRecoveryTimer;

public:
	// Sets default values for this component's properties
	UCustomMovementComponent();
	
	void Initialize(ACharacterBase* Character);
	
	void StartSprint();
	
	void StopSprint();

	void ProModeStaminaUpdate(float CurrentHealth);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void SprintTimer();

	void StaminaRecoveryDelay();
	
	void StaminaRecovery();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
};
