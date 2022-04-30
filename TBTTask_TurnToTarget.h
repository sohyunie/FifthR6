// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TBTTask_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UTBTTask_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTBTTask_TurnToTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
	
};
