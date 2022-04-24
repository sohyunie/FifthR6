// Fill out your copyright notice in the Description page of Project Settings.


#include "MWater.h"

// Sets default values
AMWater::AMWater()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MWater(TEXT
	("/Game/MyCharacter/Characters/ManOfWater.ManOfWater"));
	if (MWater.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MWater.Object);
	}

}

