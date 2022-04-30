// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTask_Special.h"
#include "BossAIController.h"
#include "MyCharacter.h"
#include "BossTank.h"
#include "BehaviorTree/BlackboardComponent.h"



UBossTask_Special::UBossTask_Special()
{
	bNotifyTick = true;
	IsSpecialAttacking = false;
}

EBTNodeResult::Type UBossTask_Special::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto BossTank = Cast<ABossTank>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == BossTank)
		return EBTNodeResult::Failed;




	BossTank->SpecialAttack();
	IsSpecialAttacking = true;
	BossTank->OnAttackEnd.AddLambda([this]()->void {
		IsSpecialAttacking = false;
		});





	return EBTNodeResult::InProgress;

}

void UBossTask_Special::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsSpecialAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}



