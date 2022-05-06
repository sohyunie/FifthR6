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

	FName WeaponSocket(TEXT("Bip-R-HandSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> WEAPON(TEXT
		("/Game/MyCharacter/Weapons/WaterSword.WaterSword"));
		if (WEAPON.Succeeded())
		{
			Weapon->SetStaticMesh(WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}
}

