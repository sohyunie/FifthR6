// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "GameFramework/Actor.h"
#include "FireBall.generated.h"



UCLASS()
class FIFTH_API AFireBall : public AActor
{
	GENERATED_BODY()

private:
	
	USoundWave* Electricity_Sound;

	//UFUNCTION()
		//void OnOverlapBegin(UPrimitiveComponent* overlappedComp, AActor* otherActor,
			//UPrimitiveComponent* otherComp, int32 OtherBodyIndex, bool bFromSweep,
			//const FHitResult& SweepResult);
	
public:	
	// Sets default values for this actor's properties
	AFireBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(EditAnywhere, Category = "Firing")
		//UNiagaraSystem* FireEffectMuzzle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(VisibleDefaultsOnly, Category = "FireBall")
		USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Capsule;

	//UPROPERTY(VisibleAnywhere)
		//UNiagaraComponent* Particle;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
		class UProjectileMovementComponent* ProjectileMovementComponent;

	void FireInDirection(const FVector& ShootDirection);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
			UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

};
