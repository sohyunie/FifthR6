// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TBTTask_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UTBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTBTTask_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
	
};
