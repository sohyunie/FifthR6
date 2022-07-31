// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGameMode.h"
#include "NetCharacter.h"

ATitleGameMode::ATitleGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	UE_LOG(LogClass, Log, TEXT("ATitleGameMode!"));

	Socket = ClientSocket::GetSingleton();
	if (Socket->ID != 0)
	{
		UE_LOG(LogClass, Log, TEXT("ServerSocket != NULL"));
	}
	else
	{
		Socket->InitSocket();
		//UE_LOG(LogClass, Log, TEXT("server addr : [%s]"), ip_addr);
		bIsConnected = Socket->Connect("211.198.122.35", 8080);
		//bIsConnected = Socket->Connect("127.0.0.1", 8080);
		UE_LOG(LogClass, Log, TEXT("server addr : 211.198.122.35  8080"));
		if (bIsConnected)
		{
			Socket->SetTitleGameMode(this);
			UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
		}
	}
}

bool ATitleGameMode::Login(const FText& Id, const FText& Pw)
{
	UE_LOG(LogClass, Log, TEXT("Try Login!"));
	if (Id.IsEmpty() || Pw.IsEmpty()) {

		UE_LOG(LogClass, Log, TEXT("IsEmpty!"));
		return false;
	}

	if (!bIsConnected) {
		UE_LOG(LogClass, Log, TEXT("Not connected!"));
		return false;
	}

	Socket->Login(Id, Pw);


	if (Socket->ID == 0) {
		UE_LOG(LogClass, Log, TEXT("Login Fail!"));
		return false;
	}

	return true;
}


void ATitleGameMode::LoginFail()
{
	UE_LOG(LogClass, Log, TEXT("Try Login Fail!"));
}

void ATitleGameMode::SetCharacter(int id)
{
	UE_LOG(LogClass, Log, TEXT("Set Character : [%d] "), id);
	Socket->SetCharacterID(id);
}

void ATitleGameMode::MoveInGame()
{
	IsMoveScene = true;
}

void ATitleGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsMoveScene)
	{
		UE_LOG(LogClass, Log, TEXT("IsMoveScene"));
		IsMoveScene = false;
		UGameplayStatics::OpenLevel(this, "BasementLab");
	}
}