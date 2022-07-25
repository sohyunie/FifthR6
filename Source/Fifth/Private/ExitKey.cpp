// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitKey.h"

// Sets default values
AExitKey::AExitKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));

	RootComponent = Body;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Key(
		TEXT("/Game/Futuristic_SciFi_Lab/Meshes/Ceiling_Light_2_SM.Ceiling_Light_2_SM"));

	if (Key.Succeeded())
	{
		Body->SetStaticMesh(Key.Object);
	}


}

// Called when the game starts or when spawned
void AExitKey::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExitKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation();

	float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));

	NewLocation.X += DeltaHeight * XValue;
	NewLocation.Y += DeltaHeight * YValue;
	NewLocation.Z += DeltaHeight * ZValue;

	RunningTime += DeltaTime;
	SetActorLocation(NewLocation);
}

void AExitKey::DestructKey()
{
	AExitKey::Destroy();

	
}
