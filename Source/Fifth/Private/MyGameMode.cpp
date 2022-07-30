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
#include "Kismet/GameplayStatics.h"
#include "NewHUD.h"

AMyGameMode::AMyGameMode(): Super()
{

	//Use our custom HUD class
	HUDClass = ANewHUD::StaticClass();

	UE_LOG(LogClass, Log, TEXT("AMyGameMode!"));
	Socket = ClientSocket::GetSingleton();

	//UE_LOG(LogTemp, Display, TEXT("START"));

	DefaultPawnClass = AWarriorOfWater::StaticClass();
	switch (Socket->CharacterID) {
	case 1:
		DefaultPawnClass = AWarriorOfFire::StaticClass();
		break;
	case 2:
		DefaultPawnClass = AWarriorOfThunder::StaticClass();
		break;
	case 3:
		DefaultPawnClass = AWarriorOfWater::StaticClass();
		break;
	}
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

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);

	NetCharacter = Cast<ANetCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AMyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (NetCharacter)
	{
		if (FMath::IsNearlyZero(NetCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}

EGamePlayState AMyGameMode::GetCurrentState() const
{
	return CurrentState;
}

void AMyGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void AMyGameMode::HandleNewState(EGamePlayState NewState)
{
	switch (NewState)
	{
	case EGamePlayState::EPlaying:
		{
			//do nothing
		}
		break;

	case EGamePlayState::EGameOver:
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
	break;

	default:
	case EGamePlayState::EUnknown:
	{
		//do nothing
	}
	break;
	}
}