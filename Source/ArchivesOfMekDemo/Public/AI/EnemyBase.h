// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class ARCHIVESOFMEKDEMO_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

private:
	// Variables
	/// References

	class AEnemyControllerBase* EnemyController;
	
	UWorld* WorldRef;

	/// Timers
	FTimerHandle HealthBarTimer;
	FTimerHandle HitReactTimer;
	FTimerHandle AttackWaitTimer;
	FTimerHandle DeathTimer;
	
	/// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AgroSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Components", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CombatRangeSphere;
	//// Behavior Tree
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree;

	/// Behavior Tree Helpers
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;
	
	/// Base Stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Health", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Health", meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;
	
	/// Combat
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(VisibleAnywhere, Category="Combat", meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;

	UPROPERTY(EditAnywhere, Category="Combat", meta = (AllowPrivateAccess = "true"))
	float AttackWaitTime;
	
	//// Hit React
	bool bCanHitReact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMax;
	
	//// Animation Stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animations", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Animations", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	FName AttackLFast;
	FName AttackRFast;
	FName AttackL;
	FName AttackR;

	//// Aesthetics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Aesthetics", meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	//// Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Weapons", meta = (AllowPrivateAccess = "true"))
	FName LeftWeaponSocket;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Weapons", meta = (AllowPrivateAccess = "true"))
	FName RightWeaponSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat|Weapons", meta=(AllowPrivateAccess="true"))
	class UBoxComponent* LeftWeaponCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat|Weapons", meta=(AllowPrivateAccess="true"))
	UBoxComponent* RightWeaponCollision;

	//// Stun
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat|Stunned", meta = (AllowPrivateAccess = "true"))
	bool bStunned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Stunned", meta = (AllowPrivateAccess = "true", ClampMin = 0, ClampMax = 1, UIMin = 0, UIMax = 1))
	float StunChance;

	//// Death
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Death", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	bool bDying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Death", meta = (AllowPrivateAccess = "true"))
	float DeathTime;
public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Overlap Overrides
	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Weapon Collision
	UFUNCTION(BlueprintCallable, Category="Combat|Weapon Collision")
	void ActivateLeftWeapon();

	UFUNCTION(BlueprintCallable, Category="Combat|Weapon Collision")
	void DeactivateLeftWeapon();

	UFUNCTION(BlueprintCallable, Category="Combat|Weapon Collision")
	void ActivateRightWeapon();

	UFUNCTION(BlueprintCallable, Category="Combat|Weapon Collision")
	void DeactivateRightWeapon();

	// Combat Helper Functions
	void DoDamage(class ACharacterBase* Character);
	void SpawnBlood(ACharacterBase* Victim, FName SocketName);

	UFUNCTION(BlueprintPure, Category="Combat")
	FName GetAttackSectionName() const;
	
	// Health Bar
	UFUNCTION(BlueprintNativeEvent, Category="Health")
	void ShowHealthBar();

	UFUNCTION(BlueprintImplementableEvent, Category="Health")
	void HideHealthBar();

	// Death
	void Die();
	
	UFUNCTION(BlueprintCallable, Category="Combat|Death")
	void FinishDeath();

	UFUNCTION()
	void DestroyEnemy();

	// Stun Functions
	UFUNCTION(BlueprintCallable, Category="Combat")
	void SetStunned(bool Stunned);

	// Combat Animations
	void PlayHitMontage(FName Section, float PlayRate = 1.f);

	UFUNCTION(BlueprintCallable, Category="Combat")
	void PlayAttackMontage(FName Section, float PlayRate);

	// Timer Callback Functions
	void ResetHitReactTimer();
	void ResetCanAttack();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
