// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ShadowSetting.generated.h"

/**
 * 
 */
UCLASS(config=Shadow)
class FIFTHSETTING_API UShadowSetting : public UObject
{
	GENERATED_BODY()

public:
	UShadowSetting();

	UPROPERTY(config)
		TArray<FSoftObjectPath> ShadowAssets;
	
};
