// Fill out your copyright notice in the Description page of Project Settings.


#include "SnowLevelStreamerActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASnowLevelStreamerActor::ASnowLevelStreamerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	RootComponent = OverlapVolume;

	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASnowLevelStreamerActor::OverlapBegins);

}

// Called when the game starts or when spawned
void ASnowLevelStreamerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASnowLevelStreamerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASnowLevelStreamerActor::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	//ANetCharacter MyCharacter = Cast<ANetCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (OtherActor == MyCharacter && LevelToLoad != "")
	{
		ANetCharacter* NetCharacter = Cast<ANetCharacter>(MyCharacter);
		FLatentActionInfo LatentInfo;
		UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
		NetCharacter->ChangeUELevel(2);
	}
}
