// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyCharacterSetting.generated.h"

/**
 * 
 */
UCLASS(config=Warrior)
class FIFTHSETTING_API UMyCharacterSetting : public UObject
{
	GENERATED_BODY()

public:
	UMyCharacterSetting();

	UPROPERTY(config)
		TArray<FSoftObjectPath> WarriorAssets;
	
};
