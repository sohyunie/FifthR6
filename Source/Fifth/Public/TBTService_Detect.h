// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTService.h"
#include "TBTService_Detect.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UTBTService_Detect : public UBTService
{
	GENERATED_BODY()

public:
	UTBTService_Detect();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		float DeltaSeconds) override;
	
};
