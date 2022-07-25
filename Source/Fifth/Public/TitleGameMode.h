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
		UFUNCTION(BlueprintCallable)
		bool Login(const FText& Id, const FText& Pw);
		UFUNCTION(BlueprintCallable)
		void LoginFail();
		UFUNCTION(BlueprintCallable)
		void SetCharacter(int id);
public:
	ATitleGameMode();
	
private:

	ClientSocket* Socket;
	bool bIsConnected;
};
