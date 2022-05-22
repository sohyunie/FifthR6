// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorOfFire.h"
#include "MyCharacterSetting.h"

// Sets default values
AWarriorOfFire::AWarriorOfFire()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> WarriorOfFire(TEXT
	("/Game/MyCharacter/Characters/WarriorOfFire.WarriorOfFire"));
	if (WarriorOfFire.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(WarriorOfFire.Object);
	}

	
	FName WeaponSocket(TEXT("Bip-R-HandSocket"));
	if (GetMesh()->DoesSocketExist(WeaponSocket))
	{
		Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WEAPON"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> WEAPON(TEXT
		("/Game/MyCharacter/Weapons/FireSword.FireSword"));
		if (WEAPON.Succeeded())
		{
			Weapon->SetStaticMesh(WEAPON.Object);
		}

		Weapon->SetupAttachment(GetMesh(), WeaponSocket);
	}
	

	

}

