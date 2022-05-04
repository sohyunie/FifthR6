// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TankSetting.generated.h"

/**
 * 
 */
UCLASS(config=Tank)
class FIFTHSETTING_API UTankSetting : public UObject
{
	GENERATED_BODY()

public:
	UTankSetting();

	UPROPERTY(config)
		TArray<FSoftObjectPath> TankAssets;
};
