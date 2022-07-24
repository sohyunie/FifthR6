// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserActor.h"
#include "BossTank.h"

// Sets default values
ALaserActor::ALaserActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));

	RootComponent = Body;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Laser(
		TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	if (Laser.Succeeded())
	{
		Body->SetStaticMesh(Laser.Object);
	}

	Dimensions = FVector(300, 0, 0);
	AxisVector = FVector(0, 0, 1);
	Multiplier = 50.f;

	//Boss = Cast<ABossTank>(ABossTank::StaticClass());

}

// Called when the game starts or when spawned
void ALaserActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaserActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector NewLocation = Boss->GetActorLocation();

	//AngleAxis += DeltaTime * Multiplier;
	
	//if (AngleAxis >= 360.0f)
	//{
		//AngleAxis = 0;

	//}

	//FVector RotateValue = Dimensions.RotateAngleAxis(AngleAxis, AxisVector);

	//NewLocation.X += RotateValue.X;
	//NewLocation.Y += RotateValue.Y;
	//NewLocation.Z += RotateValue.Z;

	//FRotator NewRotation = FRotator(0, AngleAxis, 0);

	//FQuat QuatRotation = FQuat(NewRotation);

	//SetActorLocationAndRotation(NewLocation, QuatRotation,false,0,ETeleportType::None);

}

