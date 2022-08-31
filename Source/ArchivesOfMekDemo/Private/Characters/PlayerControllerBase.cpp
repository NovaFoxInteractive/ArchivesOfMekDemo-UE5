// Copyright NovaFox Interactive L.L.C 2021
#include "Characters/PlayerControllerBase.h"

#include "Blueprint/UserWidget.h"
#include "Characters/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerControllerBase::APlayerControllerBase() :
	InventorySlots(20),
	bCanOpenInventory(true)
{
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	const auto PlayerRef = Cast<ACharacterBase>(GetCharacter());
	PlayerRef->GetInventoryComponent()->SetupArray(PlayerRef);
	
	InventoryWidgetRef = CreateWidget<UUserWidget>(this, InventoryWidget);
	if(InventoryWidgetRef)
	{
		InventoryWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
		InventoryWidgetRef->AddToViewport(1);
	}
	ensure(InventoryWidgetRef);
}

void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent == NULL)
	{
		InputComponent = NewObject<UInputComponent>(this, TEXT("PC_InputComponent0"));
		InputComponent->RegisterComponent();
	}

	InputComponent->BindAction(TEXT("InventoryToggle"), IE_Pressed, this, &APlayerControllerBase::ToggleInventory);
}

void APlayerControllerBase::ToggleInventory()
{
	if(InventoryWidgetRef && bCanOpenInventory)
	{
		if(bIsInventoryOpen)
		{
			bIsInventoryOpen = false;

			GetCharacter()->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
			GetCharacter()->EnableInput(this);
			ResetIgnoreLookInput();
			SetShowMouseCursor(false);
			InventoryWidgetRef->SetVisibility(ESlateVisibility::Collapsed);
			HideItemInfo();

			const FInputModeGameOnly InSettings;
			SetInputMode(InSettings);
		}
		else
		{
			bIsInventoryOpen = true;
		
			GetCharacter()->GetCharacterMovement()->DisableMovement();
			GetCharacter()->DisableInput(this);
			SetIgnoreLookInput(true);
			SetShowMouseCursor(true);
			InventoryWidgetRef->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			FInputModeGameAndUI InSettings;
			InSettings.SetWidgetToFocus(InventoryWidgetRef->GetCachedWidget());
			InSettings.SetHideCursorDuringCapture(true);
			InSettings.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InSettings);
		}
	}
}
