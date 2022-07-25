// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));

	RootComponent = Body;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Door(
		TEXT("/Game/NewMap/StaticMeshes/elevator_door.elevator_door"));

	if (Door.Succeeded())
	{
		Body->SetStaticMesh(Door.Object);
	}
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::DestructDoor()
{
	ADoor::Destroy();
}
