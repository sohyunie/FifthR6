// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "MyCharacter.h"
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
	UPROPERTY(VisibleAnywhere, Category = Weapon)
		UStaticMeshComponent* Weapon;

	virtual void RAttack() override;
	virtual void Fire() override;

	
	
};
