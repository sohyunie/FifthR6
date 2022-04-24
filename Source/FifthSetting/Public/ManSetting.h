// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ManSetting.generated.h"

/**
 * 
 */
UCLASS(config=Man)
class FIFTHSETTING_API UManSetting : public UObject
{
	GENERATED_BODY()

public:
	UManSetting();

	UPROPERTY(config)
		TArray<FSoftObjectPath> ManAssets;
	
};
