// Fill out your copyright notice in the Description page of Project Settings.


#include "BossDecorator_IsInRealAttack.h"
#include "BossAIController.h"
#include "MyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBossDecorator_IsInRealAttack::UBossDecorator_IsInRealAttack()
{
	NodeName = TEXT("CanRealAttack");
}

bool UBossDecorator_IsInRealAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const

{
	int x = 0;
	x = FMath::RandRange(1, 100);

	if (x <= 40)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(ABossAIController::SelectAttackNumberKey, 1);
	}
	else if(x <= 70)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(ABossAIController::SelectAttackNumberKey, 2);
	}
	else if (x <= 85)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(ABossAIController::SelectAttackNumberKey, 3);
	}
	else 
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(ABossAIController::SelectAttackNumberKey, 4);
	}

	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
		return false;

	auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ABossAIController::TargetKey));
	if (nullptr == Target)
		return false;

	bResult = (Target->GetDistanceTo(ControllingPawn) <= 900.0f);
	return bResult;
}

