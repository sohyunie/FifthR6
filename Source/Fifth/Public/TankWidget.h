// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "Blueprint/UserWidget.h"
#include "TankWidget.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UTankWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindTankStat(class UTankStatComponent* NewTankStat);

protected:
	virtual void NativeConstruct() override;
	void UpdateHPWidget();

private:
	TWeakObjectPtr<class UTankStatComponent> CurrentTankStat;

	UPROPERTY()
		class UProgressBar* HPProgressBar;
	
};
