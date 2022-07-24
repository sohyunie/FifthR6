// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGameMode.h"
#include "NetCharacter.h"

ATitleGameMode::ATitleGameMode()
{
	UE_LOG(LogClass, Log, TEXT("ATitleGameMode!"));
	Socket = ClientSocket::GetSingleton();
	Socket->InitSocket();
	//string ip_addr = "192.168.55.170";
	//UE_LOG(LogClass, Log, TEXT("server addr : [%s]"), ip_addr);
	//bIsConnected = Socket->Connect("211.198.122.35", 8080);
	bIsConnected = Socket->Connect("127.0.0.1", 8080);
	//bIsConnected = Socket->Connect("192.168.45.1", 5000);
	UE_LOG(LogClass, Log, TEXT("server addr : 211.198.122.35  8080"));
	if (bIsConnected)
	{
		UE_LOG(LogClass, Log, TEXT("IOCP Server connect success!"));
	}
}

bool ATitleGameMode::Login(const FText& Id, const FText& Pw)
{
	if (Id.IsEmpty() || Pw.IsEmpty())
		return false;

	if (!bIsConnected)
		return false;

	bool IsSuccess = Socket->Login(Id, Pw);

	if (!IsSuccess)
		return false;

	Socket->CloseSocket();
	return true;
}
