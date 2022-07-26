// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalTrigger.h"
#include "NetCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortalTrigger::APortalTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereRadius = 200.0f;
	MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MySphereComponent"));
	MyCollisionSphere->InitSphereRadius(SphereRadius);
	MyCollisionSphere->SetCollisionProfileName("Trigger");
	RootComponent = MyCollisionSphere;

	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this,
		&APortalTrigger::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<USoundWave> Portal(TEXT("SoundWave'/Game/MySound/TELEPORT.TELEPORT'"));

	if (Portal.Succeeded())
	{
		Portal_Sound = Portal.Object;
	}

}

// Called when the game starts or when spawned
void APortalTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void APortalTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	

	if (OtherActor->IsA(ANetCharacter::StaticClass()))
	{
		ABLOG(Warning, TEXT("ACCESS!!!"));

		UGameplayStatics::PlaySoundAtLocation(this, Portal_Sound, GetActorLocation());
		
		MyCharacter = Cast<AActor>(OtherActor);
		MyHit = SweepResult;
		//GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACampFire::ApplyFireDamage, 2.2f, true, 0.0f);

		
	}
}