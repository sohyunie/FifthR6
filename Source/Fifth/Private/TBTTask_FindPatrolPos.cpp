// Fill out your copyright notice in the Description page of Project Settings.


#include "TBTTask_FindPatrolPos.h"
#include "TankAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UTBTTask_FindPatrolPos::UTBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPatrolPos");
}

EBTNodeResult::Type UTBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
		return EBTNodeResult::Failed;

	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(ATankAIController::HomePosKey);
	FNavLocation NextPatrol;

	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, NextPatrol))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(ATankAIController::PatrolPosKey,
			NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;

}


