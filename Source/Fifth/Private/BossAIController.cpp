// Fill out your copyright notice in the Description page of Project Settings.


#include "BossAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName ABossAIController::HomePosKey(TEXT("HomePos"));
const FName ABossAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName ABossAIController::TargetKey(TEXT("Target"));

ABossAIController::ABossAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		BBObject(TEXT("/Game/AI/Tank/BossTank/BB_Boss.BB_Boss"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		BTObject(TEXT("/Game/AI/Tank/BossTank/BT_Boss.BT_Boss"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void ABossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BBAsset, Blackboard))
	{
		Blackboard->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());
		if (!RunBehaviorTree(BTAsset))
		{
			ABLOG(Error, TEXT("TankAIController couldn't run behavior tree!"));
		}
	}
}
