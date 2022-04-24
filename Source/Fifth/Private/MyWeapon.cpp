// Fill out your copyright notice in the Description page of Project Settings.


#include "MyWeapon.h"

// Sets default values
AMyWeapon::AMyWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
	RootComponent = Weapon;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FIRE_WEAPON(
		TEXT("/Game/MyCharacter/Weapons/FireSword.FireSword"));
	if (FIRE_WEAPON.Succeeded())
	{
		//ABLOG(Warning, TEXT("WEAPON??"));
		Weapon->SetStaticMesh(FIRE_WEAPON.Object);
	}

	Weapon->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AMyWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

