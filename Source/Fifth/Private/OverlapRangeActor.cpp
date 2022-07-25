// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlapRangeActor.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "TimerManager.h"
#include "ATank.h"
#include "BossTank.h"
#include "NetCharacter.h"

// Sets default values
AOverlapRangeActor::AOverlapRangeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereRadius = 300.0f;
	MyCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("MySphereComponent"));
	MyCollisionSphere-> InitSphereRadius(SphereRadius);
	MyCollisionSphere->SetCollisionProfileName("Trigger");
	RootComponent = MyCollisionSphere;

	MyCollisionSphere->OnComponentBeginOverlap.AddDynamic(this,
		&AOverlapRangeActor::OnOverlapBegin);

	bCanApplyDamage = false;

}

// Called when the game starts or when spawned
void AOverlapRangeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOverlapRangeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DrawDebugSphere(GetWorld(), GetActorLocation(), SphereRadius, 20, FColor::Purple, false, -1, 0, 1);

}


void AOverlapRangeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	/*if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		Destroy();
	}*/

	if (OtherActor->IsA(AATank::StaticClass()))
	{
		ABLOG(Warning, TEXT("HIT!!!"));
		bCanApplyDamage = true;
		MyCharacter = Cast<AActor>(OtherActor);
		MyHit = SweepResult;
		//GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACampFire::ApplyFireDamage, 2.2f, true, 0.0f);

		UGameplayStatics::ApplyPointDamage(OtherActor, 100.0f, OtherActor->GetActorLocation(), SweepResult, nullptr, this, nullptr);
		bCanApplyDamage = false;
		//AOverlapRangeActor::Destroy();
	}
}



