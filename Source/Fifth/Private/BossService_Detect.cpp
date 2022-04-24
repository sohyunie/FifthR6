// Fill out your copyright notice in the Description page of Project Settings.


#include "BossService_Detect.h"
#include "BossAIController.h"
#include "MyCharacter.h"
#include "BossTank.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UBossService_Detect::UBossService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBossService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;

	if (nullptr == World) return;
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);
	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			AMyCharacter* MyCharacter = Cast<AMyCharacter>(OverlapResult.GetActor());
			if (MyCharacter && MyCharacter->GetController()->IsPlayerController())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(ABossAIController::TargetKey,
					MyCharacter);
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);

				DrawDebugPoint(World, MyCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), MyCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}


