// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Fifth.h"
#include "BehaviorTree/BTDecorator.h"
#include "BossDecorator_IsInRealAttack.generated.h"

/**
 * 
 */
UCLASS()
class FIFTH_API UBossDecorator_IsInRealAttack : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBossDecorator_IsInRealAttack();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) const override;
	
};
