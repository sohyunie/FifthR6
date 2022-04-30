// Fill out your copyright notice in the Description page of Project Settings.


#include "TBTDecorator_IsInAttackRange.h"
#include "TankAIController.h"
#include "MyCharacter.h"
#include "ATank.h"
#include "BehaviorTree/BlackboardComponent.h"

UTBTDecorator_IsInAttackRange::UTBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UTBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const

{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
		return false;

	auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ATankAIController::TargetKey));
	if (nullptr == Target)
		return false;

	bResult = (Target->GetDistanceTo(ControllingPawn) <= 600.0f);
	return bResult;
}
