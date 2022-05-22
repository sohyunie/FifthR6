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
	UE_LOG(LogClass, Log, TEXT("AMyGameMode!"));
	Socket = ClientSocket::GetSingleton();
	Socket->InitSocket();
	//string ip_addr = "192.168.55.170";
	//UE_LOG(LogClass, Log, TEXT("server addr : [%s]"), ip_addr);
	bIsConnected = Socket->Connect("127.0.0.1", 5000);
	//bIsConnected = Socket->Connect("192.168.45.1", 5000);
	UE_LOG(LogClass, Log, TEXT("server addr : 127.0.0.1  5000"));
	if (bIsConnected)
	{
		UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
	}

	//UE_LOG(LogTemp, Display, TEXT("START"));

	DefaultPawnClass = AWarriorOfFire::StaticClass();
	//DefaultPawnClass = AWarriorOfWater::StaticClass();
	//DefaultPawnClass = AWarriorOfThunder::StaticClass();
	//DefaultPawnClass = AMFire::StaticClass();
	

	//DefaultPawnClass = AMWater::StaticClass();
	//DefaultPawnClass = AMThunder::StaticClass();
	PlayerControllerClass = ANetPlayerController::StaticClass();
	//PlayerControllerClass = AMyPlayerController::StaticClass();
	PlayerStateClass = AMyPlayerState::StaticClass();
}

void AMyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto MyPlayerState = Cast<AMyPlayerState>(NewPlayer->PlayerState);
	ABCHECK(nullptr != MyPlayerState);
	MyPlayerState->InitPlayerData();
	
}

