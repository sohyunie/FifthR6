// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTDecorator.h"
#include "TBTDecorator_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UTBTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UTBTDecorator_IsInAttackRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) const override;
	
};
