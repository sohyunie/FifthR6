// Fill out your copyright notice in the Description page of Project Settings.


#include "Stone.h"


// Sets default values
AStone::AStone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	Sw= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SW"));

	RootComponent = Body;
	Sw->SetupAttachment(Body);

	//Sw->SetRelativeLocation(FVector(1030.0f, 9080.0f, 150.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Stone(
		TEXT("/Game/Map/SM_MERGED_Stone1.SM_MERGED_Stone1"));

	if (Stone.Succeeded())
	{
		Body->SetStaticMesh(Stone.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BOX(
		TEXT("/Game/Map/SM_Switch.SM_Switch"));
	if (BOX.Succeeded())
	{
		Sw->SetStaticMesh(BOX.Object);
		Sw->OnComponentBeginOverlap.AddDynamic(this, &AStone::MyOverlap);
	}

	//Trigger->SetCollisionProfileName(TEXT("Coffee"));
	//Box->SetCollisionProfileName(TEXT("NoCollision"));
	Sw->SetCollisionProfileName(TEXT("LevelEvent"));

}

// Called when the game starts or when spawned
void AStone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AStone::PostInitializeComponents()
{
	Super::PostInitializeComponents();


}

void AStone::MyOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABLOG_S(Warning);
	//SetActorRotator
	SetActorRelativeRotation(GetActorRotation()+(FRotator(20.0f, 0.0f, 0.0f)));
	//TriggerON = true;
	
}


	

// Called every frame
void AStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if (TriggerON==true) {
		//SetActorLocation(GetActorLocation() + (FVector(0.0f, 0.0f, 1000.0f)));
		//ABLOG(Warning, TEXT("TRIGGERON"));
		//Body->AddWorldTransform(FTransform(FRotator(0.0f, 0.0f, 0.0f), 
			//FVector(0.0f, 0.0f, 10.0f),
			//FVector(0.0f, 0.0f, 0.0f)));
	//}
}

