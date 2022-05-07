// Fill out your copyright notice in the Description page of Project Settings.


#include "Coffee.h"
#include "Stone.h"

// Sets default values
ACoffee::ACoffee()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Coffee(
		TEXT("/Game/Map/SM_Switch.SM_Switch"));
	if (Coffee.Succeeded())
	{
		Box->SetStaticMesh(Coffee.Object);
	}

	Box->SetRelativeLocation(FVector(1030.0f, 9030.0f, 150.0f));

	Trigger->SetCollisionProfileName(TEXT("Coffee"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	
}




// Called when the game starts or when spawned
void ACoffee::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACoffee::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACoffee::OnCharacterOverlap);
	ABLOG(Warning, TEXT("OVER"));
}

void ACoffee::OnCharacterOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//ABLOG_S(Warning);

	
}

// Called every frame
void ACoffee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

