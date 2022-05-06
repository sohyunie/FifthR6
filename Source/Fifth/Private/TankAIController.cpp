// Fill out your copyright notice in the Description page of Project Settings.


#include "TankAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName ATankAIController::HomePosKey(TEXT("HomePos"));
const FName ATankAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName ATankAIController::TargetKey(TEXT("Target"));
const FName ATankAIController::SelectAttackNumberKey(TEXT("SelectAttackNumber"));

ATankAIController::ATankAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		BBObject(TEXT("/Game/AI/Tank/BB_Tank.BB_Tank"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BTObject(TEXT("/Game/AI/Tank/BT_Tank.BT_Tank"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void ATankAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void ATankAIController::RunAI()
{
	
	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, GetPawn()->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			ABLOG(Error, TEXT("TankAIController couldn't run behavior tree!"));
		}
	}
}

void ATankAIController::StopAI()
{
	auto BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (nullptr!=BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}
	
}