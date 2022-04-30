// Fill out your copyright notice in the Description page of Project Settings.


#include "TBTTask_TurnToTarget.h"
#include "TankAIController.h"
#include "ATank.h"
#include "MyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UTBTTask_TurnToTarget::UTBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UTBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ATank = Cast<AATank>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ATank)
		return EBTNodeResult::Failed;

	auto Target = Cast<AMyCharacter>(OwnerComp.GetBlackboardComponent()->
		GetValueAsObject(ATankAIController::TargetKey));
	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - ATank->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	ATank->SetActorRotation(FMath::RInterpTo(ATank->GetActorRotation(), TargetRot,
		GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}
