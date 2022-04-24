// Fill out your copyright notice in the Description page of Project Settings.


#include "MThunder.h"

// Sets default values
AMThunder::AMThunder()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MThunder(TEXT
	("/Game/MyCharacter/Characters/ManOfThunder.ManOfThunder"));
	if (MThunder.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MThunder.Object);
	}

}

