// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossTask_TurnToTarget.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UBossTask_TurnToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBossTask_TurnToTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
	
};
