// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "ClientSocket.h"
#include "GameFramework/GameModeBase.h"
#include "NetCharacter.h"
#include "MyGameMode.generated.h"

/**
 * 
 */

UENUM()
enum class EGamePlayState
{
	EPlaying, 
	EGameOver,
	EUnknown
};

UCLASS()
class FIFTH_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	ANetCharacter* NetCharacter;

	UFUNCTION(BlueprintPure, Category = "Health")
		EGamePlayState GetCurrentState() const;

	void SetCurrentState(EGamePlayState NewState);

	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	bool Login(const FText& Id, const FText& Pw);

private:
	EGamePlayState CurrentState;

	void HandleNewState(EGamePlayState NewState);
		

	ClientSocket* Socket;
	bool bIsConnected;
};
