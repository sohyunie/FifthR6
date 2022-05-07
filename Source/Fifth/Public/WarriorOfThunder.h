// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "MyCharacter.h"
#include "WarriorOfThunder.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API AWarriorOfThunder : public AMyCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWarriorOfThunder();

public:
	UPROPERTY(VisibleAnywhere, Category = Weapon)
		UStaticMeshComponent* Weapon;
	
};
