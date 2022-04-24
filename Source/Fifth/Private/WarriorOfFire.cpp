// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorOfFire.h"

// Sets default values
AWarriorOfFire::AWarriorOfFire()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WarriorOfFire(TEXT
	("/Game/MyCharacter/Characters/WarriorOfFire.WarriorOfFire"));
	if (WarriorOfFire.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(WarriorOfFire.Object);
	}

}

