// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorOfThunder.h"

// Sets default values
AWarriorOfThunder::AWarriorOfThunder()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WarriorOfThunder(TEXT
	("/Game/MyCharacter/Characters/WarriorOfThunder.WarriorOfThunder"));
	if (WarriorOfThunder.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(WarriorOfThunder.Object);
	}

}

