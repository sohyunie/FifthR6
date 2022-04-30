// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TBTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UTBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTBTTask_Attack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsAttacking = false;
	
};
