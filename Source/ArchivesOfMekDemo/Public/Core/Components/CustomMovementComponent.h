// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDodge);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARCHIVESOFMEKDEMO_API UCustomMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	// Player Reference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class ACharacterBase* PlayerRef;

	// Movement Values
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values|Sprint", meta=(AllowPrivateAccess="true"))
	float WalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Values|Sprint", meta=(AllowPrivateAccess="true"))
	float SprintSpeed;

	// Movement
	UPROPERTY(BlueprintReadWrite, Category = "Values|Dodge", meta = (AllowPrivateAccess = "true"))
	bool bIsDodging;
	UPROPERTY(BlueprintReadWrite, Category = "Values|Dodge", meta = (AllowPrivateAccess = "true"))
	bool bCanDodge;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Values|Dodge", meta = (AllowPrivateAccess = "true"))
	float TimeBetweenDodge;
	
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

	FTimerHandle DodgeResetTimer;

public:
	// Sets default values for this component's properties
	UCustomMovementComponent();

	UPROPERTY(BlueprintAssignable)
	FOnDodge OnDodge;
	
	void Initialize(ACharacterBase* Character);
	
	void StartSprint();
	
	void StopSprint();

	void ProModeStaminaUpdate(float CurrentHealth);

	bool Dodge();

	void StopDodge();

	FORCEINLINE void ResetDodge() { bCanDodge = true; }
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

	FORCEINLINE bool GetIsDodging() const { return bIsDodging; }
	
	FORCEINLINE void SetCanDodge(const bool Value) { bCanDodge = Value; }
	
};
