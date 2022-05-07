// Fill out your copyright notice in the Description page of Project Settings.


#include "MFire.h"

// Sets default values
AMFire::AMFire()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MFire(TEXT
	("/Game/MyCharacter/Characters/ManOfFire.ManOfFire"));
	if (MFire.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MFire.Object);
	}

}


