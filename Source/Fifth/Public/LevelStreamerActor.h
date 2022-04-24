// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "GameFramework/Actor.h"
#include "LevelStreamerActor.generated.h"

UCLASS()
class FIFTH_API ALevelStreamerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelStreamerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OverlapBegins(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
		FName LevelToLoad;

	/*UFUNCTION()
		void OverlapEnds(UPrimitiveComponent* OverlappedComponent, 
			AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);*/

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 레벨 스트리밍을 발동시킬 오버랩 볼륨
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* OverlapVolume;

	
};
