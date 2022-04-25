// Fill out your copyright notice in the Description page of Project Settings.


#include "BossService_RandomNumber.h"
#include "BossAIController.h"
#include "BossTank.h"
#include "BehaviorTree/BlackboardComponent.h"

/*UBossService_RandomNumber::UBossService_RandomNumber()
{
	NodeName = TEXT("CreateRandomNumber");
	Interval = 1.0f;

	bNotifyBecomeRelevant = true;
}

void UBossService_RandomNumber::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	int x = 0;
	x = FMath::RandRange(1, 100);



	if (x <= 50)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(ABossAIController::SelectAttackNumberKey, 1);
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(ABossAIController::SelectAttackNumberKey, 2);
	}

}*/


