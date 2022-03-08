// Copyright NovaFox Interactive L.L.C 2021

#include "Widgets/InteractionWidget.h"
#include "Core/Components/InteractionComponent.h"

void UInteractionWidget::UpdateInteractionWidget(UInteractionComponent* InteractionComponent)
{
	OwningInteractionComponent = InteractionComponent;
	OnUpdateInteractionWidget();
}