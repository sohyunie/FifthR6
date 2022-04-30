// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossTask_Kick.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UBossTask_Kick : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBossTask_Kick();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, float DeltaSeconds) override;

private:
	//bool IsAttacking = false;
	bool IsKicking = false;
	
};
