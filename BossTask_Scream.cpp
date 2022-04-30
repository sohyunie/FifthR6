// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTask_Scream.h"
#include "BossAIController.h"
#include "MyCharacter.h"
#include "BossTank.h"
#include "BehaviorTree/BlackboardComponent.h"



UBossTask_Scream::UBossTask_Scream()
{
	bNotifyTick = true;
	IsScreaming = false;
}

EBTNodeResult::Type UBossTask_Scream::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto BossTank = Cast<ABossTank>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == BossTank)
		return EBTNodeResult::Failed;




	BossTank->Screaming();
	IsScreaming = true;
	BossTank->OnAttackEnd.AddLambda([this]()->void {
		IsScreaming = false;
		});





	return EBTNodeResult::InProgress;

}

void UBossTask_Scream::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsScreaming)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

