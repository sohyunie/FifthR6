// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossTask_FindPatrolPos.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UBossTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBossTask_FindPatrolPos();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
	
};
