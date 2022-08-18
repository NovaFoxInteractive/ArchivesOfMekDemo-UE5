// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ARCHIVESOFMEKDEMO_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	// Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MinHealth;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float CurrentHealth;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

public:
	// Sets default values for this component's properties
	UHealthComponent();

	void UpdateHealth(float Amount);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
	FORCEINLINE bool Died() const { return (CurrentHealth <= MinHealth && !bIsDead); }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE void SetIsDead() { bIsDead = true; }
};
