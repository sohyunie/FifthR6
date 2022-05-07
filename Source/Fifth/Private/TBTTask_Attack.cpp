// Fill out your copyright notice in the Description page of Project Settings.


#include "TBTTask_Attack.h"
#include "TankAIController.h"
#include "ATank.h"

UTBTTask_Attack::UTBTTask_Attack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UTBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ATank = Cast<AATank>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ATank)
		return EBTNodeResult::Failed;
	
	ATank->Attack();
	IsAttacking = true;
	ATank->OnAttackEnd.AddLambda([this]()->void {
		IsAttacking = false;
		//ABLOG(Warning, TEXT("PLAYING!!!"));
		});

	return EBTNodeResult::InProgress;

}

void UTBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}