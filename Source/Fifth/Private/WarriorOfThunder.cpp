// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorOfThunder.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "OverlapRangeActor.h"
#include "FireBall.h"


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

	if (!FMath::IsNearlyZero(Skill, 0.001f) && bCanUseSkill)
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


			if (OvCheck)
			{
				UNiagaraSystem* ARange =
					Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
						TEXT("/Game/RangeAttack/NiagaraSystems/NS_AOE_FireColumn1.NS_AOE_FireColumn1")));
				UNiagaraFunctionLibrary::SpawnSystemAttached(ARange, OvCheck->MyCollisionSphere, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);
			}

			ABLOG(Warning, TEXT("DERIVED SUCCESS"));
		}
	}
}

void AWarriorOfThunder::Fire()
{
	Super::Fire();

	ABLOG(Warning, TEXT("DERIVED FIRE SUCCESS"));

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
		AFireBall* Projectile = World->SpawnActor<AFireBall>(AFireBall::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);

		
		if (Projectile)
		{
			UNiagaraSystem* Muzzle =
				Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
					TEXT("/Game/FireBall/NiagaraSystems/NS_Muzzle_Electric.NS_Muzzle_Electric")));
			UNiagaraFunctionLibrary::SpawnSystemAttached(Muzzle, Projectile->Capsule, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);

			UNiagaraSystem* FireEffectMuzzle =
				Cast<UNiagaraSystem>(StaticLoadObject(UNiagaraSystem::StaticClass(), NULL,
					TEXT("/Game/FireBall/NiagaraSystems/NS_Projectile_Fireball_Electric.NS_Projectile_Fireball_Electric")));
			UNiagaraFunctionLibrary::SpawnSystemAttached(FireEffectMuzzle, Projectile->Capsule, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);

			FVector LaunchDirection = MuzzleRotation.Vector();
			Projectile->FireInDirection(LaunchDirection);
		}

	}
}