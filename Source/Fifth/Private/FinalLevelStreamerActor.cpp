// Fill out your copyright notice in the Description page of Project Settings.


#include "FinalLevelStreamerActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFinalLevelStreamerActor::AFinalLevelStreamerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;

	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &AFinalLevelStreamerActor::OverlapBegins);

}

// Called when the game starts or when spawned
void AFinalLevelStreamerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFinalLevelStreamerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFinalLevelStreamerActor::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (OtherActor == MyCharacter && LevelToLoad != "")
	{
		FLatentActionInfo LatentInfo;
		UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
	}
}

