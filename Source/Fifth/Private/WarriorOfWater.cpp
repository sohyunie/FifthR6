// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorOfWater.h"

// Sets default values
AWarriorOfWater::AWarriorOfWater()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WarriorOfWater(TEXT
	("/Game/MyCharacter/Characters/WarriorOfWater.WarriorOfWater"));
	if (WarriorOfWater.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(WarriorOfWater.Object);
	}

}

