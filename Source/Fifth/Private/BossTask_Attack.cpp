// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTask_Attack.h"
#include "BossAIController.h"
#include "MyCharacter.h"
#include "BossTank.h"
#include <random>

std::random_device rd;
std::uniform_int_distribution<int> uid(0, 1);


UBossTask_Attack::UBossTask_Attack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UBossTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto BossTank = Cast<ABossTank>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == BossTank)
		return EBTNodeResult::Failed;

	
	BossTank->Attack();
	IsAttacking = true;
	BossTank->OnAttackEnd.AddLambda([this]()->void {
		IsAttacking = false;
		});
	
	
	
	

	return EBTNodeResult::InProgress;

}

void UBossTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

