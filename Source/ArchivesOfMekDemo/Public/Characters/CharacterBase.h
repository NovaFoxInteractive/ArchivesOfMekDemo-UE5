// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GlobalEnum/EAttackType.h"
#include "GlobalEnum/EAttackAnim.h"
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

	UPROPERTY(BlueprintReadOnly, Category = GameMode, meta = (AllowPrivateAccess = "true"))
	class AArchivesOfMekDemoGameModeBase* GameMode;

	// Camera Movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	// Movement
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsDodging;
	UPROPERTY(BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bCanDodge;
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float BaseWalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float SprintWalkSpeed;

	// Combat
	UPROPERTY(BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;
	
	// / Combat Block System
	UPROPERTY(BlueprintReadWrite, Category = "Combat|Blocking", meta = (AllowPrivateAccess = "true"))
	bool bIsBlocking;
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Blocking", meta = (AllowPrivateAccess = "true"))
	bool bCanBlock;

	// / Combo System
	UPROPERTY(BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	int AttackCount = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EAttackType PreviousAttack = EAttackType::EAT_None;
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EAttackType CurrentAttackType = EAttackType::EAT_None;

	// / Animations
	UPROPERTY(BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAnimPlaying = false;
	UPROPERTY(BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bAnimLoaded = false;
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	EAttackAnim AnimNum;

	// Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attributes|Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Attributes|Health", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth;
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attributes|Stamina", meta = (AllowPrivateAccess = "true"))
	float MaxStamina;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attributes|Stamina", meta = (AllowPrivateAccess = "true"))
	float CurrentStamina;

public:
	// Sets default values for this character's properties
	ACharacterBase();

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

	// / Combo System
	void INT_ComboLogic();
	bool INT_ComboNumCheck();

	// / Combat Blocking System
	void INT_Block();
	void INT_StopBlock();

	// Health
	void INT_Death();

	// Advanced Movement
	void INT_Dodge();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Getter Functions
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool GetIsDodging() const { return bIsDodging; }
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }
	FORCEINLINE bool GetIsBlocking() const { return bIsBlocking; }

	// Combat
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Combat)
	void Attack();
	// / Combo System
	UFUNCTION(BlueprintCallable, Category = "Combat|Blocking")
	void JumpAttackCheck();
	// / Combat Blocking System
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Blocking")
	void Block();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Blocking")
	void StopBlock();
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Stamina")
	void ChangeStamina(float Val);
	// Health
	UFUNCTION(BlueprintImplementableEvent, Category = Health)
	void Death();
	// Advanced Movement
	UFUNCTION(BlueprintImplementableEvent, Category = Movement)
	void Dodge();
};
