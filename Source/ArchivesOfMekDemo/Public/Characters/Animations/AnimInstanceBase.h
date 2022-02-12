// Copyright NovaFox Interactive L.L.C 2021
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstanceBase.generated.h"

/**
 * 
 */
UCLASS()
class ARCHIVESOFMEKDEMO_API UAnimInstanceBase : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UAnimInstanceBase();

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
	void UpdateAnimationProperties(float DeltaTime);

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bIsBlocking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = References, meta = (AllowPrivateAccess = "true"))
	class ACharacterBase* Pawn;
};
