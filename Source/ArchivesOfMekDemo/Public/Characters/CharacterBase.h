// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GlobalEnum/EAttackType.h"
#include "GlobalEnum/EAttackAnim.h"
#include "Core/Components/InventoryComponent.h"
#include "CharacterBase.generated.h"

UCLASS()
class ARCHIVESOFMEKDEMO_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Health", meta = (AllowPrivateAccess = "true"))
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes|Health", meta = (AllowPrivateAccess = "true"))
	class UCustomMovementComponent* CustomMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	class USphereComponent* PickupTraceCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory ,meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* Inventory;
	
	UPROPERTY(BlueprintReadOnly, Category = GameMode, meta = (AllowPrivateAccess = "true"))
	class AArchivesOfMekDemoGameModeBase* GameMode;

	// Camera Movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;
	
	// Damage Aesthetics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Damage|Aesthetics", meta = (AllowPrivateAccess = "true"))
	class USoundCue* MeleeImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat|Damage|Aesthetics", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BloodParticles;
public:
	// Sets default values for this character's properties
	ACharacterBase();

	// Take Combat Damage
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Movement
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Sprint();
	void StopSprint();
	// Camera Movement
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	// Combat
	void INT_LightAttack();
	void INT_HeavyAttack();

	// / Combat Blocking System
	void Block();
	void StopBlock();

	// Health
	void INT_Death();

	// Advanced Movement
	void Dodge();

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Getter Functions
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCustomMovementComponent* GetCustomMovement() const { return CustomMovementComponent; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return Inventory; }
	FORCEINLINE USphereComponent* GetPickupTraceCollision() const { return PickupTraceCollision; }

	FORCEINLINE USoundCue* GetMeleeImpactSound() const { return MeleeImpactSound; }
	FORCEINLINE UParticleSystem* GetBloodParticles() const { return BloodParticles; }
	
	// Health
	UFUNCTION(BlueprintImplementableEvent, Category = Health)
	void Death();
};
