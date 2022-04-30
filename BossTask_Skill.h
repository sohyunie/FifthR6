// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BossTask_Skill.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UBossTask_Skill : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBossTask_Skill();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsSkillUsing = false;
	
	
};
