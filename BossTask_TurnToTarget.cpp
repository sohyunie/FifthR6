// Fill out your copyright notice in the Description page of Project Settings.


#include "BossTask_TurnToTarget.h"
#include "BossAIController.h"
#include "BossTank.h"
#include "MyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBossTask_TurnToTarget::UBossTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBossTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ATank = Cast<ABossTank>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ATank)
		return EBTNodeResult::Failed;

	auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->
		GetValueAsObject(ABossAIController::TargetKey));
	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - ATank->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ATank->SetActorRotation(FMath::RInterpTo(ATank->GetActorRotation(), TargetRot,
		GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}

