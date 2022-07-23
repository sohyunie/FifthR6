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
#include "NetPlayerController.h"
#include "NetCharacter.h"

AMyGameMode::AMyGameMode()
{
	DefaultPawnClass = AWarriorOfThunder::StaticClass();

	PlayerControllerClass = ANetPlayerController::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto MyPlayerState = Cast<AMyPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != MyPlayerState);
	MyPlayerState->InitPlayerData();
}
