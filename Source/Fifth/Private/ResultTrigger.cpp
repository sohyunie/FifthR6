// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultTrigger.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NetCharacter.h"
#include "Blueprint/UserWidget.h"
#include "NetPlayerController.h"

// Sets default values
AResultTrigger::AResultTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereRadius = 100.0f;
	MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MySphereComponent"));
	MyCollisionSphere->InitSphereRadius(SphereRadius);
	MyCollisionSphere->SetCollisionProfileName("Trigger");
	RootComponent = MyCollisionSphere;

	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this,
		&AResultTrigger::OnOverlapBegin);


	static ConstructorHelpers::FClassFinder<UUserWidget> Result(
		TEXT("/Game/UI/UI_Result.UI_Result_C"));

	ResultWidgetClass = Result.Class;

	ResultCheck = false;

}

// Called when the game starts or when spawned
void AResultTrigger::BeginPlay()
{
	Super::BeginPlay();

	
	//if (ResultWidgetClass)
	//{
		

	//}
	
}

// Called every frame
void AResultTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AResultTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	/*if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		Destroy();
	}*/

	if (OtherActor->IsA(ANetCharacter::StaticClass()))
	{
		ResultCheck = true;

		
	}
}