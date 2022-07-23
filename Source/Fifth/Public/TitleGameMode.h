// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "CoreMinimal.h"
#include "ClientSocket.h"
#include "GameFramework/GameModeBase.h"
#include "TitleGameMode.generated.h"

UCLASS()
class FIFTH_API ATitleGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ATitleGameMode();

	bool Login(const FText& Id, const FText& Pw);
private:

	ClientSocket* Socket;
	bool bIsConnected;
};
