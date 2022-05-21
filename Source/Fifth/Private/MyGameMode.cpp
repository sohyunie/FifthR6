// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "MyCharacter.h"
#include "ManOfFire.h"
#include "WarriorOfFire.h"
#include "WarriorOfWater.h"
#include "WarriorOfThunder.h"
#include "MFire.h"
#include "MWater.h"
#include "MThunder.h"
#include "MyPlayerController.h"
#include "MyPlayerState.h"

AMyGameMode::AMyGameMode()
{
	DefaultPawnClass = AWarriorOfFire::StaticClass();
	//DefaultPawnClass = AWarriorOfWater::StaticClass();
	//DefaultPawnClass = AWarriorOfThunder::StaticClass();
	//DefaultPawnClass = AMFire::StaticClass();
	

	//DefaultPawnClass = AMWater::StaticClass();
	//DefaultPawnClass = AMThunder::StaticClass();
	PlayerControllerClass = AMyPlayerController::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto MyPlayerState = Cast<AMyPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != MyPlayerState);
	MyPlayerState->InitPlayerData();
	
}

