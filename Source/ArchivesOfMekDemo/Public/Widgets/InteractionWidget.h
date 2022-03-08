// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARCHIVESOFMEKDEMO_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Interaction)
	void UpdateInteractionWidget(class UInteractionComponent* InteractionComponent);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUpdateInteractionWidget();

private:
	UPROPERTY(BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"), meta = (ExposeOnSpawn))
	class UInteractionComponent* OwningInteractionComponent;
	
};
