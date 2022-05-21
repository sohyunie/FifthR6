// Fill out your copyright notice in the Description page of Project Settings.


#include "TBTService_Detect.h"
#include "TankAIController.h"
#include "MyCharacter.h"
#include "ATank.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

UTBTService_Detect::UTBTService_Detect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UTBTService_Detect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
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
				ABLOG(Warning, TEXT("aaaaaaaa"));
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(ATankAIController::TargetKey,
					MyCharacter);
				//ABLOG(Warning, TEXT("%s"), &ATankAIController::TargetKey.ToString());
				//if (ATankAIController::TargetKey == TEXT("Target"))
				/*FName aa = ATankAIController::TargetKey;
				FString bb = aa.ToString();
				ABLOG(Warning, TEXT("%s"), &bb);*/
				DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);

				DrawDebugPoint(World, MyCharacter->GetActorLocation(), 10.0f, FColor::Blue, false, 0.2f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), MyCharacter->GetActorLocation(), FColor::Blue, false, 0.2f);
				return;
			}
		}
	}

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
}
