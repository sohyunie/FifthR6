// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossTask_Special.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UBossTask_Special : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBossTask_Special();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsSpecialAttacking = false;
	
};
