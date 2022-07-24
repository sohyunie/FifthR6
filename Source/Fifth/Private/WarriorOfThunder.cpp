// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorOfThunder.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "OverlapRangeActor.h"

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

void AWarriorOfThunder::RAttack()
{
	Super::RAttack();

	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);

	FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
	FRotator MuzzleRotation = CameraRotation;

	MuzzleRotation.Pitch += 10.0f;
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		AOverlapRangeActor* OvCheck = World->SpawnActor<AOverlapRangeActor>(AOverlapRangeActor::StaticClass(),
			MuzzleLocation + GetControlRotation().Vector() * 1000.f, MuzzleRotation, SpawnParams);

		UNiagaraSystem* ARange =
			Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
				TEXT("/Game/RangeAttack/NiagaraSystems/NS_AOE_FireColumn.NS_AOE_FireColumn")));
		UNiagaraFunctionLibrary::SpawnSystemAttached(ARange, OvCheck->MyCollisionSphere, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);


		ABLOG(Warning, TEXT("DERIVED SUCCESS"));
	}
}

