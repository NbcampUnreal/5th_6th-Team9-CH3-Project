// Fill out your copyright notice in the Description page of Project Settings.

#include "P9PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

AP9PlayerController::AP9PlayerController()
	:InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	ZoomCameraAction(nullptr),
	FreeLookAction(nullptr),
	ForwardRollAction(nullptr),
	InteractAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr)
{
}

void AP9PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

UUserWidget* AP9PlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AP9PlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

	}
}
	


