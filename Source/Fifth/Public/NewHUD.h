// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "GameFramework/HUD.h"
#include "NewHUD.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API ANewHUD : public AHUD
{
	GENERATED_BODY()
	

public:
	ANewHUD();

	//virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:
	

	UPROPERTY(EditAnywhere, Category = "Health")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Health")
		class UUserWidget* CurrentWidget;

};
