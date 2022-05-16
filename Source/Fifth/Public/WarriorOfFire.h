// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "NetCharacter.h"
#include "WarriorOfFire.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API AWarriorOfFire : public ANetCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWarriorOfFire();

public:
	// [TODO] 임시주석
	UPROPERTY(VisibleAnywhere, Category = Weapon)
		UStaticMeshComponent* Weapon;


private:
	
	
};
