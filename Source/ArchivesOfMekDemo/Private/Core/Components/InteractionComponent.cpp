// Copyright NovaFox Interactive L.L.C 2021


#include "Core/Components/InteractionComponent.h"

#include "Log.h"
#include "Characters/CharacterBase.h"
#include "Widgets/InteractionWidget.h"

UInteractionComponent::UInteractionComponent() :
	InteractionTime(0.f), InteractionDistance(200.f),
	InteractableNameText(FText::FromString("Interactable Object")),
	InteractableActionText(FText::FromString("Interact"))
{
	UActorComponent::SetComponentTickEnabled(false);

	Space = EWidgetSpace::Screen;
	DrawSize = FIntPoint(600, 100);
	bDrawAtDesiredSize = true;

	UActorComponent::SetActive(true);
	SetHiddenInGame(true);
}

void UInteractionComponent::BeginFocus(ACharacterBase* Character)
{
	if (!IsActive() || !GetOwner() || !Character)
	{
		return;
	}
	
	OnBeginFocus.Broadcast(Character);

	SetHiddenInGame(false);

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);
	for (const auto& VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(true);
		}
	}

	RefreshWidget();
}

void UInteractionComponent::EndFocus(ACharacterBase* Character)
{
	UE_LOG(LogArchivesOfMek, Warning, TEXT("Ending Focus!"));
	
	OnEndFocus.Broadcast(Character);

	SetHiddenInGame(true);

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(UPrimitiveComponent::StaticClass(), Components);
	for (const auto& VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(false);
		}
	}
}

void UInteractionComponent::BeginInteract(ACharacterBase* Character)
{
	if (CanInteract(Character))
	{
		Interactor = Character;
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(ACharacterBase* Character)
{
	Interactor = nullptr;
	OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(ACharacterBase* Character)
{
	if (CanInteract(Character))
		OnInteract.Broadcast(Character);
}

void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractableNameText = NewNameText;
	RefreshWidget();
}

void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractableActionText = NewActionText;
	RefreshWidget();
}

void UInteractionComponent::Deactivate()
{
	Super::Deactivate();
	
	if (Interactor)
	{
		EndFocus(Interactor);
		EndInteract(Interactor);
	}

	Interactor = nullptr;
}

bool UInteractionComponent::CanInteract(const ACharacterBase* Character) const
{
	if (IsActive() && GetOwner() != nullptr && Character != nullptr)
		return true;
	return false;
}

void UInteractionComponent::RefreshWidget()
{
	UE_LOG(LogArchivesOfMek, Warning, TEXT("Widget Refreshed!"));
	if (!bHiddenInGame)
		if (UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
			InteractionWidget->UpdateInteractionWidget(this);
}

float UInteractionComponent::GetInteractPercentage()
{
		if (Interactor)
			if (Interactor && Interactor->IsInteracting())
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);

	return 0.0f;
}