// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalEnum/EAttackAnim.h"
#include "GlobalEnum/EAttackType.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttack, UAnimationAsset*, Animation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBlock);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopBlock);

USTRUCT(BlueprintType)
struct FAttackAnimations
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EAttackAnim, UAnimationAsset*> Animations;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARCHIVESOFMEKDEMO_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TArray<TSubclassOf<class AWeapon>> Weapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeapon> EquippedWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TMap<TSubclassOf<AWeapon>, FAttackAnimations> AttackAnimations;

	class ACharacterBase* PlayerRef;

	UPROPERTY(BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bIsAttacking;
	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	bool bCanAttack;

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

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnAttack OnAttack;
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBlock OnBlock;
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnStopBlock OnStopBlock;

	void EquipWeaponFromIndex(int Index);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void AddWeapon(const TSubclassOf<AWeapon> WeaponToAdd) { Weapons.Add(WeaponToAdd); }

	UFUNCTION(BlueprintCallable, Category = "Combat|Blocking")
	void JumpAttackCheck();

	void ResetAttack();

	UFUNCTION(BlueprintCallable)
	void Attack(EAttackType Attack);

	// / Combat Blocking System
	void Block();
	void StopBlock();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// / Combo System
	void ComboLogic();
	FORCEINLINE bool ComboNumCheck() const { return AttackCount > 1 ?  true :  false; }

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UAnimationAsset* GetAttackAnimation(EAttackAnim AnimationType);

	FORCEINLINE void Initialize(ACharacterBase* Character) { PlayerRef = Character; }

	FORCEINLINE void SetIsAttacking() { bIsAttacking = true; }
	FORCEINLINE void SetValuesFalse() { bIsAttacking = false; bIsBlocking = false; }

	FORCEINLINE bool GetCanAttack() const { return !bCanAttack || bAnimLoaded; }

	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }
	FORCEINLINE bool GetIsBlocking() const { return bIsBlocking; }
};
