// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class ACharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class ACharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class ACharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class ACharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class ACharacterBase*, Character);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ARCHIVESOFMEKDEMO_API UInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	float InteractionTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	float InteractionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	FText InteractableNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	FText InteractableActionText;

public:
	FORCEINLINE float GetInteractionDistance() const { return InteractionDistance; }
	FORCEINLINE float GetInteractionTime() const { return InteractionTime; }

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndInteract OnEndInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndFocus OnEndFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnInteract OnInteract;

protected:
	virtual void Deactivate() override;

	bool CanInteract(const class ACharacterBase* Character) const;

	class ACharacterBase* Interactor;

public:
	void RefreshWidget();
	
	UInteractionComponent();

	void BeginFocus(class ACharacterBase* Character);
	void EndFocus(class ACharacterBase* Character);

	void BeginInteract(class ACharacterBase* Character);
	void EndInteract(class ACharacterBase* Character);
	
	void Interact(class ACharacterBase* Character);

	UFUNCTION(BlueprintPure, Category = Interaction)
	float GetInteractPercentage();

	UFUNCTION(BlueprintCallable, Category = Interaction)
	void SetInteractableNameText(const FText& NewNameText);

	UFUNCTION(BlueprintCallable, Category = Interaction)
	void SetInteractableActionText(const FText& NewActionText);

	UFUNCTION(BlueprintCallable, Category=Interaction)
	void SetInteractionTime(const float NewInteractionTime) { InteractionTime = NewInteractionTime; }

	UFUNCTION(BlueprintCallable, Category=Interaction)
	void SetInteractionDistance(const float NewInteractionDistance) { InteractionDistance = NewInteractionDistance; }
};
