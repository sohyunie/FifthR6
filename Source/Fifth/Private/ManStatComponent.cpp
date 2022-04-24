// Fill out your copyright notice in the Description page of Project Settings.


#include "ManStatComponent.h"
#include "MyGameInstance.h"

// Sets default values for this component's properties
UManStatComponent::UManStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	Level = 1;
	// ...
}


// Called when the game starts
void UManStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UManStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UManStatComponent::SetNewLevel(int32 NewLevel)
{
	auto MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != MyGameInstance);
	CurrentStatData = MyGameInstance->GetManData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		CurrentHP = CurrentStatData->MaxHP;
	}
	else {
		ABLOG(Error, TEXT("Man Level (%d) data doesn't exist"), NewLevel);
	}
}

void UManStatComponent::SetDamage(float NewDamage)
{
	ABCHECK(nullptr != CurrentStatData);
	CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP);
	if (CurrentHP <= 0.0f)
	{
		OnHPIsZero.Broadcast();
	}
}

float UManStatComponent::GetAttack()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}