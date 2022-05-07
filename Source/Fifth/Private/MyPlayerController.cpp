// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyHUDWidget.h"
#include "MyPlayerState.h"
#include "ManOfFire.h"

AMyPlayerController::AMyPlayerController()
{
	static ConstructorHelpers::FClassFinder<UMyHUDWidget> UI_HUD_C(
		TEXT("/Game/UI/UI_HUD.UI_HUD_C"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}

	ChangePawn = false;
}

void AMyPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ABLOG_S(Warning);
}


void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	HUDWidget = CreateWidget<UMyHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();
	

	auto MyPlayerState = Cast<AMyPlayerState>(PlayerState);
	ABCHECK(nullptr != MyPlayerState);
	HUDWidget->BindPlayerState(MyPlayerState);
	MyPlayerState->OnPlayerStateChanged.Broadcast();

	//HUDWidget->AddToViewport();
}

UMyHUDWidget* AMyPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void AMyPlayerController::OnPossess(APawn* aPawn)
{
	ABLOG_S(Warning);
	
	Super::OnPossess(aPawn);
	//ABLOG(Warning, TEXT("NOW PAWN: %s"), aPawn->GetName());


}