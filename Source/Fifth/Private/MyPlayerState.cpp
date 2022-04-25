// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"

AMyPlayerState::AMyPlayerState()
{
	CharacterLevel = 1;
	GameScore = 0;
}

int32 AMyPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 AMyPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

void AMyPlayerState::InitPlayerData()
{
	SetPlayerName(TEXT("devX"));
	CharacterLevel = 5;
	GameScore = 0;
}

