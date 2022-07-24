// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveCom.h"

// Sets default values
ASaveCom::ASaveCom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));

	RootComponent = Body;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Com(
		TEXT("/Game/SM_MERGED_Com.SM_MERGED_Com"));

	if (Com.Succeeded())
	{
		Body->SetStaticMesh(Com.Object);
	}

}

// Called when the game starts or when spawned
void ASaveCom::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASaveCom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

