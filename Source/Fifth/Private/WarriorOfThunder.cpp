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

	FName WeaponSocket(TEXT("Bip-R-HandSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> WEAPON(TEXT
		("/Game/MyCharacter/Weapons/ThunderSword.ThunderSword"));
		if (WEAPON.Succeeded())
		{
			Weapon->SetStaticMesh(WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}
}

