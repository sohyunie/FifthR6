// Fill out your copyright notice in the Description page of Project Settings.


#include "NewHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Blueprint/UserWidget.h"

ANewHUD::ANewHUD()
{
	
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(
		TEXT("/Game/UI/Health_UI.Health_UI_C"));

	HUDWidgetClass = HealthBarObj.Class;
}

void ANewHUD::BeginPlay()
{
	Super::BeginPlay();
	//ABLOG(Warning, TEXT("HUDBegin"));
	if (HUDWidgetClass != nullptr)
	{
		//ABLOG(Warning, TEXT("HUD"));
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget)
		{
			//ABLOG(Warning, TEXT("HUDView"));
			CurrentWidget->AddToViewport();
		}
	}
}
