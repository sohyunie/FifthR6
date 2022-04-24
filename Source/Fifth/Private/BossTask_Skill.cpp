// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTask_Skill.h"
#include "BossAIController.h"
#include "MyCharacter.h"
#include "BossTank.h"
#include "BehaviorTree/BlackboardComponent.h"



UBossTask_Skill::UBossTask_Skill()
{
	bNotifyTick = true;
	IsSkillUsing = false;
}

EBTNodeResult::Type UBossTask_Skill::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto BossTank = Cast<ABossTank>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == BossTank)
		return EBTNodeResult::Failed;




	BossTank->Skill();
	IsSkillUsing = true;
	BossTank->OnAttackEnd.AddLambda([this]()->void {
		IsSkillUsing = false;
		});





	return EBTNodeResult::InProgress;

}

void UBossTask_Skill::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsSkillUsing)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

