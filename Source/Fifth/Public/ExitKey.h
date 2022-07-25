// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "GameFramework/Actor.h"
#include "ExitKey.generated.h"

UCLASS()
class FIFTH_API AExitKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExitKey();

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Body;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float RunningTime;
	

	UPROPERTY(EditAnywhere, Category = Movement);
	float XValue;

	UPROPERTY(EditAnywhere, Category = Movement);
	float YValue;

	UPROPERTY(EditAnywhere, Category = Movement);
	float ZValue=100;

	UFUNCTION()
		void DestructKey();

};
