// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "ClientSocket.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	bool Login(const FText& Id, const FText& Pw);

private:

	ClientSocket* Socket;
	bool bIsConnected;
};
