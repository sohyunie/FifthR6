// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowStatComponent.h"
#include "MyGameInstance.h"

// Sets default values for this component's properties
UShadowStatComponent::UShadowStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	Level = 1;

	// ...
}


// Called when the game starts
void UShadowStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UShadowStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level);
}

void UShadowStatComponent::SetNewLevel(int32 NewLevel)
{
	auto MyGameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != MyGameInstance);
	CurrentStatData = MyGameInstance->GetShadowData(NewLevel);
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		CurrentHP = CurrentStatData->MaxHP;
	}
	else {
		ABLOG(Error, TEXT("Shadow Level (%d) data doesn't exist"), NewLevel);
	}
}

void UShadowStatComponent::SetDamage(float NewDamage)
{
	ABCHECK(nullptr != CurrentStatData);
	CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP);
	if (!isDead && CurrentHP <= 0.0f)
	{
		isDead = true;
		OnHPIsZero.Broadcast();
	}
}

float UShadowStatComponent::GetAttack()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);
	return CurrentStatData->Attack;
}

