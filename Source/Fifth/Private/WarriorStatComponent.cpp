// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorStatComponent.h"
#include "MyGameInstance.h"
#include "MyCharacter.h"

// Sets default values for this component's properties
UWarriorStatComponent::UWarriorStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
	Level = 1;
}


// Called when the game starts
void UWarriorStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UWarriorStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UWarriorStatComponent::SetNewLevel(int32 NewLevel)
{
	auto MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != MyGameInstance);
	CurrentStatData = MyGameInstance->GetWarriorData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		CurrentHP = CurrentStatData->MaxHP;
	}
	else {
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}

void UWarriorStatComponent::SetDamage(float NewDamage)
{
	ABCHECK(nullptr != CurrentStatData);
	CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP);
	if (CurrentHP <= 0.0f)
	{
		OnHPIsZero.Broadcast();
	}
}

float UWarriorStatComponent::GetAttack()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);


	return CurrentStatData->Attack;
}

float UWarriorStatComponent::GetSAttack()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);


	return CurrentStatData->SAttack;
}