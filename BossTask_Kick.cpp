// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTask_Kick.h"
#include "BossAIController.h"
#include "MyCharacter.h"
#include "BossTank.h"



UBossTask_Kick::UBossTask_Kick()
{
	bNotifyTick = true;
	IsKicking = false;
}

EBTNodeResult::Type UBossTask_Kick::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto BossTank = Cast<ABossTank>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == BossTank)
		return EBTNodeResult::Failed;




	BossTank->Kick();
	IsKicking = true;
	BossTank->OnAttackEnd.AddLambda([this]()->void {
		IsKicking = false;
		});





	return EBTNodeResult::InProgress;

}

void UBossTask_Kick::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsKicking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

