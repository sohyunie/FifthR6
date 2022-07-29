// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "GameFramework/Actor.h"
#include "NetPlayerController.h"
#include "ResultTrigger.generated.h"

UCLASS()
class FIFTH_API AResultTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResultTrigger();

	UPROPERTY(EditAnywhere)
		AActor* MyCharacter;

	bool ResultCheck;

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

	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> ResultWidgetClass;

	// declare widget
	class UUserWidget* ResultInfoWidget;
	UPROPERTY(EditAnywhere)
		class ANetPlayerController* Con;

};
