// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "GameFramework/Actor.h"
#include "PortalTrigger.generated.h"

UCLASS()
class FIFTH_API APortalTrigger : public AActor
{
	GENERATED_BODY()
	

public:	
	// Sets default values for this actor's properties
	APortalTrigger();


	UPROPERTY(EditAnywhere)
		AActor* MyCharacter;

	UPROPERTY(EditAnywhere)
		FHitResult MyHit;

	
private:
	USoundWave* Portal_Sound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* MyCollisionSphere;

	float SphereRadius;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult);

};
