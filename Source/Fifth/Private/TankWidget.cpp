// Fill out your copyright notice in the Description page of Project Settings.


#include "TankWidget.h"
#include "TankStatComponent.h"
#include "Components/ProgressBar.h"

void UTankWidget::BindTankStat(UTankStatComponent* NewTankStat)
{
	ABCHECK(nullptr!= NewTankStat);

	CurrentTankStat = NewTankStat;
	NewTankStat->OnHPChanged.AddUObject(this, &UTankWidget::UpdateHPWidget);
		
}

void UTankWidget::NativeConstruct()
{
	Super::NativeConstruct();
	HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));
	ABCHECK(nullptr != HPProgressBar);
	UpdateHPWidget();
}

void UTankWidget::UpdateHPWidget()
{
	if (CurrentTankStat.IsValid())
	{
		if (nullptr != HPProgressBar)
		{
			HPProgressBar->SetPercent(CurrentTankStat->GetHPRatio());
		}
	}
}
